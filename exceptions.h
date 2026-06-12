#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class Exception : public std::exception {
protected:
    std::string m_error;
public:
    Exception(const std::string& error);
    const char* what() const noexcept override;
};

class DataStructureException : public Exception {
public:
    DataStructureException(const std::string& msg);
};

class LogicException : public Exception {
public:
    LogicException(const std::string& msg);
};

class FileException : public Exception {
public:
    FileException(const std::string& msg);
};

class InvalidInputException : public Exception {
public:
    InvalidInputException(const std::string& msg);
};

#endif