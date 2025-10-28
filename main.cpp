#include <iostream>
#include <fstream>
#include <string>
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


typedef struct {
    int numCPU;
    std:: string schedulingAlgorithm;
    int timeQuantum;
    int batchFreq;
    int minCommand;
    int maxCommand;
    int delayTime;
} Config;

class Process{
public:
    std::string name;
    int coreAssigned;
    int currentInstruction;
    bool isFinished;
    time_t startTime;
    time_t endTime;

    Process()

};

class Screen {

};
int main(){
    std::string command = "";
    std::cout << ascii_art << header << "\n--------------------------------------\n" << std::flush;
    std::cout << "\n\nroot:\\>";
    std::cin >> command;
    if(command == "initialize"){
        
    }
    else{
        exit(0);
    }
    return 0;
}