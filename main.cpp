#include <iostream>

int main(){
    std:: string header= "Welcome to CSOPESY!\n\nGroup Developer: \nCastillo,Marvien Angel \nHerrera,Mikaela Gabrielle \nJimenez, Jaztin Jacob \nRegindin, Sean Adrien \n\nVersion Date:\n";
    std:: string ascii_art = R"( 
  ___ ___   ___  ____  ____  _____  __
 / __/ __| / _ \|  _ \|  __|/ __\ \/ /
| |  \__ \| | | | |_| |  _| \__ \\  /
| |__ __| | |_| |  __/| |___ __| | |
 \___|___/ \___/|_|   |_____|___/|_|
)";
    std:: string main_header = header + ascii_art;
    std:: cout << main_header; 
    std::string command;
    int speed;
    std::string text = "This is a scrolling marquee text";
    do{
        std:: cout << text;
        std:: cout << " " << (40-text.size());
        text += std::string(40-text.size(),' ');
        std:: cout << "\nNew Text: " << text;
        std:: cin >> command;
        if (command == "help"){
            std:: cout << "Available Commands:\nstart_marquee - starts the marquee \"animation\"\nstop_marquee - stops the marquee \"animation\"\nset_text - accepts a text input and displays it as a marquee\nset_speed - sets the marquee animation refresh in milliseconds\nexit - terminates the console\n";
        }
        else if(command == "start_marquee"){
            std:: cout << "Marquee started!\n";
        }
        else if(command == "stop_marquee"){
            std:: cout << "Marquee stopped!\n";
        }
        else if(command == "set_text"){
            std:: cout << "Enter text: ";
            std:: cin >> text;
            std:: cin.ignore(); // Clear the newline character from the input buffer
            
            std:: cout << "Text set to: " << text << "\n";
        }
        else if(command == "set_speed"){
            std:: cout << "Enter speed in milliseconds: ";
            std:: cin >> speed;
            std:: cout << "Speed set to: " << speed << " ms\n";
        }
        else if (command == "exit"){
            std:: cout << "Exiting the program. Goodbye!\n";
        }
    }while(command != "exit");
    return 0;
}