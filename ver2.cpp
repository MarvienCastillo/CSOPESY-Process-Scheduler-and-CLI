#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <queue>

// --- Shared State and Thread Control ---
std::atomic<bool> is_running{true};
std::atomic<bool> marquee_running{false};
std::string marquee_window = "";
std::atomic<int> marquee_speed{200}; // default 200 ms
std:: string header= "Welcome to CSOPESY!\n\nGroup Developer: \nCastillo,Marvien Angel \nHerrera,Mikaela Gabrielle \nJimenez, Jaztin Jacob \nRegindin, Sean Adrien \n\nVersion Date:\n";
    std:: string ascii_art = R"( 
  ___ ___   ___  ____  ____  _____  __
 / __/ __| / _ \|  _ \|  __|/ __\ \/ /
| |  \__ \| | | | |_| |  _| \__ \\  /
| |__ __| | |_| |  __/| |___ __| | |
 \___|___/ \___/|_|   |_____|___/|_|)";

 std:: string command_prompt = "\n\n\n\n\n\n\n\n\n\n\n\nCommand > ";

/**
 * We will use three threads:
 * 1. Keyboard Handler Thread (reads user input)
 * 2. Marquee Logic Thread (updates marquee position)
 * 3. Display Thread (renders marquee and prompt)
 */

// The command interpreter and display thread share this variable.
std::string prompt_display_buffer = "";
std::mutex prompt_mutex;

// Shared state for the keyboard handler and command interpreter.
std::queue<std::string> command_queue;
std::mutex command_queue_mutex;

// The marquee logic thread and display thread share this variable.
std::string marquee_display_buffer = "This is a scrolling marquee text! ";
std::mutex marquee_to_display_mutex;

// Display parameters
int display_width = 40; //change niyo yung width kung naliliitan kayo
int marquee_pos = 0;

// --- Utility Function ---
void gotoxy(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

// --- Thread Functions ---
void keyboard_handler_thread_func() {
    std::string command_line;
    while (is_running) {
        std::getline(std::cin, command_line);
        if (!command_line.empty()) {
            gotoxy(12,27);
            std::cout << std::string(command_line.size(),' ') << std::flush; // clear input in the command line
            std::unique_lock<std::mutex> lock(command_queue_mutex);
            command_queue.push(command_line);
        }
    }
}

void marquee_logic_thread_func() {
    int i = 0;
    
    while (is_running) {
        if (marquee_running) {
            std::string text;
            {
                std::unique_lock<std::mutex> lock(marquee_to_display_mutex);
                text = marquee_display_buffer;
            }

            if (!text.empty()) {
                std::string padded = std::string(display_width, ' ') + text + std::string(display_width, ' ');

                // Step 2: extract sliding window
                std::string window = padded.substr(i, display_width);

                // Step 3: advance index
                i++;
                if (i > (int)padded.size() - display_width) {
                    i = 0; // restart scroll
                }

                // Step 4: update display buffer
                {
                    std::unique_lock<std::mutex> lock(prompt_mutex);
                    prompt_display_buffer = window;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(marquee_speed));
    }
}


void display_thread_func() {
    const int refresh_rate_ms = 50;
    while (is_running) {
        {
            std::unique_lock<std::mutex> lock(prompt_mutex);
            gotoxy(0, 22);
            std::cout << std::setw(display_width) << std::left << prompt_display_buffer << std::flush;
            gotoxy(12,27); // for the input line
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
    }
}

void command_process(const std::string& command_line) {
    std::istringstream iss(command_line);
    std::string cmd;
    iss >> cmd;

    if(cmd == "help"){
        std::cout << "Available commands:\n"
                  << "  help          - show this help message\n"
                  << "  start_marquee - start marquee animation\n"
                  << "  stop_marquee  - stop marquee animation\n"
                  << "  set_text      - set marquee text\n"
                  << "  set_speed     - set marquee animation speed (ms)\n"
                  << "  exit          - exit emulator\n";
    } else if(cmd == "start_marquee"){
        marquee_running = true;
    } else if(cmd == "stop_marquee"){
        marquee_running = false;
    } else if (cmd == "set_text") {
        std::string new_text;
        std::getline(iss, new_text);
        if (!new_text.empty() && new_text[0] == ' ') new_text.erase(0, 1);
        if (!new_text.empty()) {
            std::unique_lock<std::mutex> lock(marquee_to_display_mutex);
            marquee_display_buffer = new_text;
        }

    } else if (cmd == "set_speed") {
        int speed;
        if (iss >> speed && speed > 0) {
            marquee_speed = speed;
        } else {
            std::cout << "Invalid speed.\n";
        }
    } else if (cmd == "exit") {
        is_running = false;
    } else {
        std::cout << "Unknown command. Type 'help' for a list of commands.\n";
    }
}

int main() {
    std::thread marquee_logic_thread(marquee_logic_thread_func);
    std::thread display_thread(display_thread_func);
    std::thread keyboard_handler_thread(keyboard_handler_thread_func);

    //COMMAND INTERPRETER
    std::cout << header+ascii_art+command_prompt<< std::flush; 
    while(is_running) {
        std::string command_line;
        {
            std::unique_lock<std::mutex> lock(command_queue_mutex);
            if (!command_queue.empty()) {
                command_line = command_queue.front();
                command_queue.pop();
            }
        }

        if (!command_line.empty()) {
            command_process(command_line);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Join threads
    if (marquee_logic_thread.joinable()) marquee_logic_thread.join();
    if (display_thread.joinable()) display_thread.join();
    if (keyboard_handler_thread.joinable()) keyboard_handler_thread.join();


    std::cout << "\n\nGoodbye!\n";
    return 0;
}