#include <windows.h>

#include <iostream>
#include <string>

int main()
{
    std::cout << "guoke demo app\nPID: " << GetCurrentProcessId()
              << "\nArchitecture: " << sizeof(void*) * 8 << "-bit\n"
              << "Passive test target. Press Enter to exit.\n" << std::flush;

    std::string input;
    std::getline(std::cin, input);
    return 0;
}
