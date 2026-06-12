#pragma once
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "time.h"

class Validator {
public:
    static int getValidatedInt(const char* prompt, int min, int max);
    static int getValidatedHours();
    static int getValidatedMinutes();
    static Time inputTimeHM();
};

#endif