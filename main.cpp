#include <iostream>
bool is_running = false;
std::string ascii_art = R"(
  ____ ____   ____  _____  _______ _________   __
 / ___/ ___| / _  \|  _  \|  _____/  ____\  \ / /
| |   \___ \| | |  | |_)  |   __| \____  \\  V /
| |___ ___) | |_|  |  ___/|  |____ ____)  ||  |
 \____|____/ \____/|_|    |_______|______/ |__|

--------------------------------------------------
)";
std::string header = "Welcome to CSOPESY Emulator!\n\nGroup Developers:\nCastillo, Marvien Angel\nHerrera, Mikaela Gabrielle\nJimenez, Jaztin Jacob\nRegindin, Sean Adrien\n\nLast Updated: 10-15-2025\n";
int main(){
    std::cout << ascii_art << header << "\n--------------------------------------\n" << std::flush;
    return 0;
}