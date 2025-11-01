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
    Process(int pID, string name,int coreAssigned,int maxIns,int minIns){
        this->pID = pID;
        this->name = name;
        this->coreAssigned = coreAssigned; // for scheduling algorithm ata to?
        this->totalInstruction = rand() % (maxIns - minIns + 1) + minIns; // ito sabi ni bff hehehhe
        this->currentInstruction = 0; // initialize
        isFinished= false; 
        this->startTime = time(nullptr); // to get the curr date and time (ctime library)
    }
    void fcfs(){

    }

    void roundRobin(int quantum){

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
public: 

    // im lowkey confused sa scheduler-start like?
    void createProcess(){
        if(!is_initialized){
            printNotInitialized();
            return;
        }
        
        string name = "P";
        
        int pID = processList.size() + 1;
        if(pID < 10)
            name += "0";
        name = name + to_string(pID); 
        processList.emplace_back(pID,name,config.numCPU,config.maxCommand,config.minCommand);
        cout << "Process  " << name << " (ID: " << pID << ") created on Core " << config.numCPU << " with "
             << processList.back().totalInstruction << " instructions." << endl;
    }
    // Create Process
    void createProcess(const string& name){ // it means that this name cannot be modified and passed by reference
        if(!is_initialized){
            printNotInitialized();
            return;
        }
        
        int pID = processList.size() + 1;
        processList.emplace_back(pID,name,config.numCPU,config.maxCommand,config.minCommand);
        cout << "Process  " << name << " (ID: " << pID << ") created on Core " << config.numCPU << " with "
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

        // Calculate actual CPU usage
        int runningProcesses = 0;
        for(auto& p: processList){
            if(!p.isFinished) runningProcesses++;
        }
        
        cpuUsed = min(runningProcesses, config.numCPU);
        float cpuUtilization = (cpuUsed / (float)config.numCPU) * 100.0f;

        cout << "CPU Utilization: " << cpuUtilization << "%" << endl;
        cout << "Cores used: " << cpuUsed << endl;
        cout << "Cores available: " << to_string(config.numCPU) << endl << endl;
        
        cout << "---------------------------------------------\n";
        cout << "Running processes: \n";
        for(auto& p: processList){
            if(!p.isFinished) {
                // Properly set up timestamps 
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);

                cout << left << setw(15) << p.name 
                 << setw(30) << dateBuffer
                 << "Core: " << setw(10) << p.coreAssigned 
                 << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        cout << "Finished processes: \n";
        for(auto& p: processList){
            if(p.isFinished) {
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);
                
                cout << left << setw(15) << p.name 
                    << setw(15) << dateBuffer
                    << "Finished" << setw(15) << " "
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }

    }

    void schedulerStart(){
        if(!is_initialized){
            printNotInitialized();
            return;
        }

        schedulerActive = true;
        cout << "Scheduler running...\n";

        // background thread so CLI will stay responsive
        thread([this]() {
            int nextProcessTick = config.batchFreq;
        
            while(schedulerActive){
                cpuCycles++; 
                return;
            }

        }).detach();
    }

    void schedulerStop()
    {
        schedulerActive = false;
        cout << "Scheduler stopped!";
    }

    void reportUtil(){
        ofstream Logs("csopesy-log.txt");
        if(!Logs.is_open()){
            cout << "Error writing report.\n";
            return;
        }
        if(processList.empty()){
            Logs << "No processes exist right now\n";
            return;
        }
        
        // Calculate actual CPU usage
        int runningProcesses = 0;
        for(auto& p: processList){
            if(!p.isFinished) runningProcesses++;
        }
        
        cpuUsed = min(runningProcesses, config.numCPU);
        float cpuUtilization = (cpuUsed / (float)config.numCPU) * 100.0f;
        
        Logs << "\n------------------------------------------\n";
        Logs << "CPU Utilization: " << fixed << setprecision(2) << cpuUtilization << "%\n";
        Logs << "Cores used: " << cpuUsed << "\n";
        Logs << "Cores available: " << config.numCPU << "\n";
        
        Logs << "---------------------------------------------\n";
        Logs << "Running processes:\n";
        for(auto& p: processList){
            if(!p.isFinished){
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);
                
                Logs << left << setw(15) << p.name 
                    << setw(35) << dateBuffer
                    << "Core: " << setw(10) << p.coreAssigned 
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        
        Logs << "\nFinished processes:\n";
        for(auto& p: processList){
            if(p.isFinished){
                struct tm timeinfo;
                localtime_s(&timeinfo, &p.startTime);
                
                char dateBuffer[32];
                strftime(dateBuffer, sizeof(dateBuffer), "(%m/%d/%Y %I:%M:%S%p)", &timeinfo);
                
                Logs << left << setw(15) << p.name 
                    << setw(35) << dateBuffer
                    << "Finished" << setw(10) << " "
                    << p.currentInstruction << "/" << p.totalInstruction << "\n";
            }
        }
        
        Logs.close();
        cout << "Report generated at " << std::filesystem::current_path() << "/csopesy-log.txt" << endl;
    }
};

void initializeSystem(){
    ifstream configFile;
    configFile.open("config.txt");
    if(!configFile){
        cout << "Unable to open config file!";
        exit(1); // terminate with error
    }
    while(!configFile.eof()){
        configFile >> config.numCPU;
        configFile >> config.schedulingAlgorithm;
        configFile >> config.timeQuantum;
        configFile >> config.batchFreq;
        configFile >> config.minCommand;
        configFile >> config.maxCommand;
        configFile >> config.delayTime;
    }
    configFile.close();
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
        }
        else if(command.compare("screen -s") >= 0){
            if(is_initialized){
                string name = command.substr(10);
                screen.createProcess(name);
            }
            else{
                printNotInitialized();
            }
        }
        else if(command.compare("screen -r") >= 0){
            if(is_initialized){
                string name = command.substr(10);
                screen.showProcess(name);
            }
            else{
                printNotInitialized();
            }
        }
        else if(command.compare("screen -ls") == 0){
            if(is_initialized){
                screen.screenList();
            }
            else{
                printNotInitialized();
            }
        }
        else if(command.compare("scheduler-start") == 0){
            screen.schedulerStart();
        }
        else if(command.compare("scheduler-stop") == 0){
            screen.schedulerStop();
        }
        else if(command.compare("report-util") == 0 ){
            screen.reportUtil();
        }
        else if(command == "exit"){
            exit(0);
        }
    }
    return 0;
}