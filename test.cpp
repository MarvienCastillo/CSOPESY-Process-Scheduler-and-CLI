#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>
#include <atomic>
#include <queue>
#include <mutex>
#include <cstdlib>

std::atomic<bool> is_running{true};
std::atomic<bool> marquee_running{false};
std::string prompt_display_buffer = "";
std::mutex prompt_mutex;

// Shared state for the keyboard handler and command interpreter.
std::queue<std::string> command_queue;
std::mutex command_queue_mutex;

// The marquee logic thread and display thread share this variable.
std::string marquee_display_buffer = "";
std::mutex marquee_to_display_mutex;

const int MARQUEE_LINE = 0; // top line for marquee
const int INPUT_LINE   = 2; // fixed line for input
int consoleWidth = 80;

void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearLine(int y, int consoleWidth) {
    setCursorPosition(0, y);
    std::cout << std::string(consoleWidth, ' ');
    setCursorPosition(0, y);
}

void keyboard_handler_thread_func() {
    std::string command_line;
    while (is_running) {
        std::getline(std::cin, command_line);
        if (!command_line.empty()) {
            std::unique_lock<std::mutex> lock(command_queue_mutex);
            command_queue.push(command_line);
            lock.unlock();
        }
    }
}


void display_thread_func(std::string text) {
    int i = 0;
    while (is_running) {
        if (marquee_running) {
            clearLine(MARQUEE_LINE, consoleWidth);

            int start = i - consoleWidth + 1;
            int end = i;

            setCursorPosition(0, MARQUEE_LINE);
            for (int j = start; j <= end; ++j) {
                if (j >= 0 && j < (int)text.length()) {
                    std::cout << text[j];
                } else {
                    std::cout << ' ';
                }
            }
            std::cout.flush();
            setCursorPosition(0, INPUT_LINE);
            std::cout << "> ";
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            i++;
            if (i > text.length() + consoleWidth) {
                i = 0;
            }
        
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

int main() {
    std::string text = "This is a scrolling marquee text! ";

    std::thread keyboard_handler_thread(keyboard_handler_thread_func);
    std::thread display_thread(display_thread_func, text);

    while (is_running) {
    
        std::string command_line;
        {
            std::unique_lock<std::mutex> lock(command_queue_mutex);
            if (!command_queue.empty()) {
                command_line = command_queue.front();
                command_queue.pop();
            }
        
        }

        if (!command_line.empty()) {
            if (command_line == "exit") {
                is_running = false;
            } else if (command_line == "start_marquee") {
                marquee_running = true;
            } else if (command_line == "stop_marquee") {
                marquee_running = false;
        
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    if (keyboard_handler_thread.joinable()) keyboard_handler_thread.join();
    if (display_thread.joinable()) display_thread.join();
    return 0;
}
