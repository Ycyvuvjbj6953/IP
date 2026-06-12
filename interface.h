#ifndef INTERFACE_H
#define INTERFACE_H
#include "lift.h"
#include "validator.h"

class Interface {
private:
    Lift modeling;
    bool configured;
    int floors;

    void displayMainMenu() const;
    void displayInputSubmenu() const;
    void handleKeyboardInput();
    void handleFileInput();
    void handleGenerateInput();
    void handleReset();
    void handleAddRequests();
    void demonstrateExceptions();

public:
    Interface();
    void run();
};

#endif