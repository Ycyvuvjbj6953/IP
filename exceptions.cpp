#include "exceptions.h"

Exception::Exception(const std::string& error)
    : m_error{ error } {
}

const char* Exception::what() const noexcept {
    return m_error.c_str();
}

DataStructureException::DataStructureException(const std::string& msg)
    : Exception{ std::string("Ошибка структуры данных: ") + msg } {
}

LogicException::LogicException(const std::string& msg)
    : Exception{ std::string("Ошибка моделирования: ") + msg } {
}

FileException::FileException(const std::string& msg)
    : Exception{ std::string("Ошибка файла: ") + msg } {
}

InvalidInputException::InvalidInputException(const std::string& msg)
    : Exception{ std::string("Ошибка ввода: ") + msg } {
}