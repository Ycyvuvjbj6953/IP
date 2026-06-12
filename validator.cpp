#include "validator.h"
#include <iostream>
#include <limits>
int Validator::getValidatedInt(const char* prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "ОШИБКА: Введите целое число." << std::endl;
            continue;
        }
        if (value < min || value > max) {
            std::cerr << "ОШИБКА: Значение от " << min << " до " << max << "." << std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}
int Validator::getValidatedHours() {
    int hours;
    while (true) {
        std::cout << "  Часы (0-23): ";
        std::cin >> hours;
        if (std::cin.fail() || hours < 0 || hours > 23) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "  ОШИБКА: Введите часы от 0 до 23." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return hours;
    }
}
int Validator::getValidatedMinutes() {
    int minutes;
    while (true) {
        std::cout << "  Минуты (0-59): ";
        std::cin >> minutes;
        if (std::cin.fail() || minutes < 0 || minutes > 59) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "  ОШИБКА: Введите минуты от 0 до 59." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return minutes;
    }
}

Time Validator::inputTimeHM() {
    int hours = getValidatedHours();      // сначала часы
    int minutes = getValidatedMinutes();  // потом минуты
    return Time(hours, minutes, 0);
}