# CSOPESY Marquee Console

## Project Information
**Course:** CSOPESY (Introduction to Operation Systems)
**Project:** Marquee Console

## Group Members
- Castillo, Marvien Angel
- Herrera, Mikaela Gabrielle
- Jimenez, Jaztin Jacob
- Regindin, Sean Adrien

## Entry Point
**Main File:** main.cpp
**Location:** CSOPESY-Marquee-console\main.cpp

## System Requirements
- Windows Operating System (uses Windows-specific APIs)
- C++ Compiler with C++11 support (MinGW/GCC recommended)
- Threading support (pthread)

## Compilation Instructions

### Using g++ (Recommended):
```bash
g++ -std=c++11 ver4.cpp -o ver4.exe
```

## Running the Program
After successful compilation, run:
```bash
ver4.exe
```

## Features
- **Multi-threaded Architecture**: Uses 3 threads for keyboard input, marquee logic, and display
- **Real-time Input**: Character-by-character input handling with backspace support
- **Scrolling Marquee**: Animated text display with customizable speed
- **Thread-safe Operations**: Proper mutex synchronization for console access
- **Command Interface**: Interactive command-line interface

## Available Commands
- `help` - Show available commands
- `start_marquee` - Start marquee animation
- `stop_marquee` - Stop marquee animation
- `set_text <text>` - Set custom marquee text
- `set_speed <ms>` - Set animation speed in milliseconds
- `exit` - Exit the program

## File Structure
```
CSOPESY-Marquee-console/
├── main.cpp          # Final implementation (MAIN ENTRY POINT)
└── README.md         # Project documentation
```

## Notes
- The program automatically starts with the marquee running
- Console cursor is hidden during execution for better visual experience
- Windows-specific functions require compilation on Windows platform
- If display issues occur, try running in a standard Windows Command Prompt or expanding the terminal