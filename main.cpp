#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <mutex>
std::string ascii_art = R"(
  ____ ____   ____  _____  _______ _________   __
 / ___/ ___| / _  \|  _  \|  _____/  ____\  \ / /
| |   \___ \| | |  | |_)  |   __| \____  \\  V /
| |___ ___) | |_|  |  ___/|  |____ ____)  ||  |
 \____|____/ \____/|_|    |_______|______/ |__|

--------------------------------------------------
)";
std::string header = "Welcome to CSOPESY Emulator!\n\nGroup Developers:\nCastillo, Marvien Angel\nHerrera, Mikaela Gabrielle\nJimenez, Jaztin Jacob\nRegindin, Sean Adrien\n\nLast Updated: 10-15-2025\n";
#define MAX_PROCESS 120

using namespace std;
typedef struct {
    int numCPU;
    string schedulingAlgorithm;
    int timeQuantum;
    int batchFreq;
    int minCommand;
    int maxCommand;
    int delayTime;
} Config;

// global variables
bool is_initialized = false;
Config config;

class Process{
public:
    string name;
    int pID;
    int coreAssigned;
    int totalInstruction;
    int currentInstruction;
    bool isFinished;
    time_t startTime;
    time_t endTime;

public:
    // constructor
    Process(int pID, string name,int maxIns,int minIns){
        this->pID = pID;
        this->name = name;
        this->coreAssigned = -1; // -1 means not assigned to any core yet
        this->totalInstruction = rand() % (maxIns - minIns + 1) + minIns; // ito sabi ni bff hehehhe
        this->currentInstruction = 0; // initialize
        isFinished= false; 
        this->startTime = time(nullptr); // to get the curr date and time (ctime library)
    }
};

void printNotInitialized() {
    cout << "Please initialize the system first!" << endl;
}

class Screen {
private:
    vector<Process> processList; // kinda same with java's ArrayList<Process> processList
    bool schedulerActive = false;
    int cpuCycles = 0;
    int cpuUsed = 0;
    int cpuAvail = 0;
    vector<thread> cpuThreads;
    mutex processListMutex;

    void fcfs(int coreID){
        while (schedulerActive){
            Process* curr = nullptr;

            // find next process
            {
                lock_guard<mutex> lock(processListMutex);
                for (auto& p : processList) {
                    if (!p.isFinished && p.coreAssigned == -1) {
                        p.coreAssigned = coreID;
                        curr = &p;
                        break;
                    }
                }
            }

            if (curr) {
                // Execute process until completion (FCFS)
                while (curr->currentInstruction < curr->totalInstruction 
                    && schedulerActive) {
                    
                    this_thread::sleep_for(chrono::milliseconds(config.delayTime));
                    
                    {
                        lock_guard<mutex> lock(processListMutex);
                        curr->currentInstruction++;

                        if (curr->currentInstruction >= curr->totalInstruction) {
                            curr->isFinished = true;
                            curr->endTime = time(nullptr);  // Record actual finish time
                            cout << "\nProcess " << curr->name << " finished on Core " << coreID << "!" << endl;
                        }
                    }
                }
                
                // Release core when finished
                {
                    lock_guard<mutex> lock(processListMutex);
                    if (curr->isFinished) {
                        curr->coreAssigned = -1;
                    }
                }
            } else {
                // No process available, idle
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }
    }

    void roundRobin(int coreID){
        while (schedulerActive){
            Process* curr = nullptr;

            // find next process
            {
                lock_guard<mutex> lock(processListMutex);
                for (auto& p : processList) {
                    if (!p.isFinished && p.coreAssigned == -1) {
                        p.coreAssigned = coreID;
                        curr = &p;
                        break;
                    }
                }
            }

            if (curr) {
                int quantum = 0;

                while (quantum < config.timeQuantum 
                    && curr->currentInstruction < curr->totalInstruction 
                    && schedulerActive) {
                    
                    this_thread::sleep_for(chrono::milliseconds(config.delayTime));
                    
                    {
                        lock_guard<mutex> lock(processListMutex);
                        curr->currentInstruction++;
                        quantum++;

                        if (curr->currentInstruction >= curr->totalInstruction) {
                            curr->isFinished = true;
                            curr->endTime = time(nullptr);  // Record actual finish time
                            cout << "\nProcess " << curr->name << " finished on Core " << coreID << "!" << endl;
                        }
                    }
                }
                
                // Release core ALWAYS (even if not finished - for Round Robin)
                {
                    lock_guard<mutex> lock(processListMutex);
                    curr->coreAssigned = -1;
                }
            } else {
                // No process available, idle
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }
    }
public: 

    // im lowkey confused sa scheduler-start like?
    void runScreen(){
        string screenCommand = "";
        bool flag = true;
        while(flag){
            cout << "\n\nroot:\\> ";
            getline(cin, screenCommand);
            
            if(screenCommand.find("screen -s ") == 0) {
                if(is_initialized){
                    string name = screenCommand.substr(10);
                    if (name.empty()) {
                        cout << "Please provide a process name.\n";
                    } else {
                        createProcess(name);
                        processScreen(name); // Enter the process screen
                    }
                }
                else{
                    printNotInitialized();
                }
            }
            else if(screenCommand.find("screen -r ") == 0) {
                if(is_initialized){
                    string name = screenCommand.substr(10);
                    if (name.empty()) {
                        cout << "Please provide a process name.\n";
                    } else {
                        processScreen(name); // Reattach to existing process
                    }
                }
                else{
                    printNotInitialized();
                }
            }
            else if(screenCommand == "screen -ls") {
                if(is_initialized){
                    screenList();
                }
                else{
                    printNotInitialized();
                }
            }
            else if(screenCommand == "scheduler-start") {
                schedulerStart();
            }
            else if(screenCommand == "scheduler-stop") {
                schedulerStop();
            }
            else if(screenCommand == "report-util") {
                reportUtil();
            }
            else if(screenCommand == "exit") {
                flag = false;
            }
            else if(!screenCommand.empty()) {
                cout << "Unknown command: " << screenCommand << "\n";
            }
        }
    }

    void createProcess(){
        if(!is_initialized){
            printNotInitialized();
            return;
        }
        
        lock_guard<mutex> lock(processListMutex);
        string name = "P";
        
        int pID = processList.size() + 1;
        if(pID < 10)
            name += "0";
        name = name + to_string(pID); 
        processList.emplace_back(pID, name, config.maxCommand, config.minCommand);
        
        cout << "Process " << name << " (ID: " << pID << ") created with "
            << processList.back().totalInstruction << " instructions." << endl;
    }
    // Create Process
    void createProcess(const string& name){ // it means that this name cannot be modified and passed by reference
        if(!is_initialized){
            printNotInitialized();
            return;
        }
        
        lock_guard<mutex> lock(processListMutex);
        int pID = processList.size() + 1;
        processList.emplace_back(pID, name, config.maxCommand, config.minCommand);
        
        cout << "Process " << name << " (ID: " << pID << ") created with "
            << processList.back().totalInstruction << " instructions." << endl;
    }
    // Show process/es
    void showProcess(string name){
        
        int i;
        int index;
        bool flag = 0;
        for(i=0;i<processList.size();i++){
            if(processList[i].name == name){
                cout << "Process " << processList[i].name << " found!" << endl;
                flag = 1;
            }
            
        }
        if(flag == 0){
            cout << "Process " << name << " not found";
        }
    }

    void screenList(){
        if(processList.empty()){
            cout << "No processes exist right now\n";
            return;
        }

        lock_guard<mutex> lock(processListMutex);
        
        // Calculate actual CPU usage
        int runningProcesses = 0;
        for(auto& p: processList){
            if(!p.isFinished && p.coreAssigned != -1) runningProcesses++;
        }
        
        cpuUsed = runningProcesses;
        cpuAvail = config.numCPU - cpuUsed;
        float cpuUtilization = (cpuUsed / (float)config.numCPU) * 100.0f;

        cout << "\nCPU Utilization: " << fixed << setprecision(2) << cpuUtilization << "%\n";
        cout << "Cores used: " << cpuUsed << "\n";
        cout << "Cores available: " << cpuAvail << "\n\n";
        
        cout << "---------------------------------------------\n";
        cout << "Running processes:\n";
        for(auto& p: processList){
            if(!p.isFinished) {
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);

                string coreStr = (p.coreAssigned == -1) ? "N/A" : to_string(p.coreAssigned);
                
                cout << left << setw(15) << p.name 
                    << setw(35) << dateBuffer
                    << "Core: " << setw(10) << coreStr
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        
        cout << "\nFinished processes:\n";
        for(auto& p: processList){
            if(p.isFinished) {
                struct tm startInfo, endInfo;
                localtime_s(&startInfo, &p.startTime);
                localtime_s(&endInfo, &p.endTime);  // Use endTime instead
                
                char startBuffer[32], endBuffer[32];
                strftime(startBuffer, sizeof(startBuffer), "(%m/%d/%Y %I:%M:%S%p)", &startInfo);
                strftime(endBuffer, sizeof(endBuffer), "(%m/%d/%Y %I:%M:%S%p)", &endInfo);
                
                cout << left << setw(15) << p.name 
                    << setw(35) << startBuffer
                    << setw(35) << endBuffer  // Show finish time
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        cout << "---------------------------------------------\n";
    }

    void schedulerStart(){
        if(!is_initialized){
            printNotInitialized();
            return;
        }

        if (schedulerActive) {
            cout << "Scheduler is already running!\n";
            return;
        }

        schedulerActive = true;
        
        // Determine which algorithm to use based on config
        if (config.schedulingAlgorithm == "\"fcfs\"" || config.schedulingAlgorithm == "fcfs") {
            cout << "Scheduler started with FCFS on " << config.numCPU << " cores.\n";
            
            // Create one thread per CPU core running FCFS
            for (int i = 0; i < config.numCPU; i++) {
                cpuThreads.emplace_back(&Screen::fcfs, this, i);
            }
        } 
        else if (config.schedulingAlgorithm == "\"rr\"" || config.schedulingAlgorithm == "rr") {
            cout << "Scheduler started with Round Robin (Quantum: " << config.timeQuantum 
                << ") on " << config.numCPU << " cores.\n";
            
            // Create one thread per CPU core running Round Robin
            for (int i = 0; i < config.numCPU; i++) {
                cpuThreads.emplace_back(&Screen::roundRobin, this, i);
            }
        }
        else {
            cout << "Unknown scheduling algorithm: " << config.schedulingAlgorithm << "\n";
            cout << "Please use 'fcfs' or 'rr' in config.txt\n";
            schedulerActive = false;
        }
    }

    void schedulerStop(){
        if (!schedulerActive) {
            cout << "Scheduler is not running!\n";
            return;
        }
        
        schedulerActive = false;
        cout << "Stopping scheduler...\n";
        
        // Wait for all CPU threads to finish
        for (auto& t : cpuThreads) {
            if (t.joinable()) {
                t.join();
            }
        }
        cpuThreads.clear();
        
        cout << "Scheduler stopped!\n";
    }

    void reportUtil(){
        ofstream Logs("csopesy-log.txt");
        if(!Logs.is_open()){
            cout << "Error writing report.\n";
            return;
        }
        if(processList.empty()){
            Logs << "No processes exist right now\n";
            Logs.close();
            return;
        }
        
        lock_guard<mutex> lock(processListMutex);
        
        // Calculate actual CPU usage
        int runningProcesses = 0;
        for(auto& p: processList){
            if(!p.isFinished && p.coreAssigned != -1) runningProcesses++;
        }
        
        cpuUsed = runningProcesses;
        cpuAvail = config.numCPU - cpuUsed;
        float cpuUtilization = (cpuUsed / (float)config.numCPU) * 100.0f;
        
        Logs << "\n------------------------------------------\n";
        Logs << "CPU Utilization: " << fixed << setprecision(2) << cpuUtilization << "%\n";
        Logs << "Cores used: " << cpuUsed << "\n";
        Logs << "Cores available: " << cpuAvail << "\n";
        
        Logs << "---------------------------------------------\n";
        Logs << "Running processes:\n";
        for(auto& p: processList){
            if(!p.isFinished){
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);

                string coreStr = (p.coreAssigned == -1) ? "N/A" : to_string(p.coreAssigned);
                
                Logs << left << setw(15) << p.name 
                    << setw(35) << dateBuffer
                    << "Core: " << setw(10) << coreStr
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        
        Logs << "\nFinished processes:\n";
        for(auto& p: processList){
            if(p.isFinished){
                struct tm startInfo, endInfo;
                localtime_s(&startInfo, &p.startTime);
                localtime_s(&endInfo, &p.endTime);  // Use endTime
                
                char startBuffer[32], endBuffer[32];
                strftime(startBuffer, sizeof(startBuffer), "(%m/%d/%Y %I:%M:%S%p)", &startInfo);
                strftime(endBuffer, sizeof(endBuffer), "(%m/%d/%Y %I:%M:%S%p)", &endInfo);
                
                Logs << left << setw(15) << p.name 
                    << setw(35) << startBuffer
                    << setw(35) << endBuffer  // Show finish time
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        
        Logs.close();
        cout << "Report generated at " << std::filesystem::current_path() << "/csopesy-log.txt" << endl;
    }

    void processScreen(string processName) {
        // Find the process
        Process* targetProcess = nullptr;
        {
            lock_guard<mutex> lock(processListMutex);
            for (auto& p : processList) {
                if (p.name == processName) {
                    targetProcess = &p;
                    break;
                }
            }
        }

        if (targetProcess == nullptr) {
            cout << "Process " << processName << " not found.\n";
            return;
        }

        // Check if process has already finished
        if (targetProcess->isFinished) {
            cout << "Process " << processName << " has already finished execution.\n";
            return;
        }

        cout << "\n----------------------------------------------\n";
        cout << "Process: " << targetProcess->name << "\n";
        cout << "Entering process screen. Type 'process-smi' for details or 'exit' to return.\n";
        cout << "----------------------------------------------\n";

        string processCommand;
        bool inProcessScreen = true;

        while (inProcessScreen) {
            cout << "\nroot:\\" << processName << "> ";
            getline(cin, processCommand);

            if (processCommand == "process-smi") {
                lock_guard<mutex> lock(processListMutex);
                
                cout << "\n----------------------------------------------\n";
                cout << "Process: " << targetProcess->name << "\n";
                cout << "ID: " << targetProcess->pID << "\n";
                
                struct tm timeinfo;
                localtime_s(&timeinfo, &targetProcess->startTime);
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "%m/%d/%Y, %I:%M:%S %p", &timeinfo);
                
                cout << "Created: " << dateBuffer << "\n";
                cout << "\nCurrent instruction line: " << targetProcess->currentInstruction << "\n";
                cout << "Lines of code: " << targetProcess->totalInstruction << "\n";
                
                if (targetProcess->isFinished) {
                    cout << "\nFinished!\n";
                    cout << "----------------------------------------------\n";
                    inProcessScreen = false; // Auto-exit when finished
                    cout << "\nProcess has completed. Returning to main menu...\n";
                } else {
                    cout << "----------------------------------------------\n";
                }
            }
            else if (processCommand == "exit") {
                inProcessScreen = false;
                cout << "\nReturning to main menu...\n";
                cout << "----------------------------------------------\n";
            }
            else if (!processCommand.empty()) {
                cout << "Unknown command. Available commands: process-smi, exit\n";
            }
        }
    }
};

void initializeSystem() {
        ifstream file("config.txt");
        if (!file.is_open()) {
            cout << "Error: config.txt not found.\n";
            return;
        }

        file >> ws;
        file >> ws; // skip whitespace
        file >> ws;

        file.seekg(0); // start from beginning
        string key;
        while (file >> key) {
            if (key == "num-cpu") file >> config.numCPU;
            else if (key == "scheduler") file >> config.schedulingAlgorithm;
            else if (key == "quantum-cycles") file >> config.timeQuantum;
            else if (key == "batch-process-freq") file >> config.batchFreq;
            else if (key == "min-ins") file >> config.minCommand;
            else if (key == "max-ins") file >> config.maxCommand;
            else if (key == "delay-per-exec") file >> config.delayTime;
        }

        file.close();
        is_initialized = true;

        cout << "System initialized successfully!\n";
        cout << "CPUs: " << config.numCPU
             << " | Scheduler: " << config.schedulingAlgorithm
             << " | Quantum: " << config.timeQuantum << "\n";
    }


int main(){
    srand(time(0));
    string command = "";
    cout << ascii_art << header << "\n--------------------------------------\n" << flush;
    Screen screen;
    while(true){
        cout << "\n\nroot:\\> ";
        getline(cin,command);
        if(command == "initialize" || command == "init"){
            initializeSystem();
            if(is_initialized){
                screen.runScreen();
            }
            else{
                printNotInitialized();
            }
        }
        else if(command == "exit"){
            exit(0);
        }
        else{
            printNotInitialized();
        }
    }
    return 0;
}