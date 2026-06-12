#include "interface.h"
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    try {
        Interface app;
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "\nКРИТИЧЕСКАЯ ОШИБКА " << e.what() << std::endl;
        return 1;
    }
    return 0;
}