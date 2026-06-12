#include "request.h"
#include <sstream>

Request::Request() : id(0), callTime(), fromFloor(1), toFloor(1) {}

Request::Request(int id, const Time& time, int from, int to)
    : id(id), callTime(time), fromFloor(from), toFloor(to) {
}

int Request::getId() const { return id; }
Time Request::getCallTime() const { return callTime; }
int Request::getFromFloor() const { return fromFloor; }
int Request::getToFloor() const { return toFloor; }

std::string Request::toString() const {
    std::stringstream ss;
    ss << "Заявка #" << id << ": " << callTime.toString()
        << ", Откуда: " << fromFloor
        << ", Куда: " << toFloor;
    return ss.str();
}