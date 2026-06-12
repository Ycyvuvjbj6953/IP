#ifndef REQUEST_H
#define REQUEST_H

#include "time.h"
#include <string>

class Request {
private:
    int id;
    Time callTime;
    int fromFloor;
    int toFloor;

public:
    Request();
    Request(int id, const Time& time, int from, int to);

    int getId() const;
    Time getCallTime() const;
    int getFromFloor() const;
    int getToFloor() const;
    std::string toString() const;
};

#endif