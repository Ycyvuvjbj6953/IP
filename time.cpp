#include "time.h"
#include <iomanip>
#include <sstream>

Time::Time() : hours(0), minutes(0), seconds(0) {}

Time::Time(int h, int m, int s) : hours(h), minutes(m), seconds(s) {
    if (seconds >= 60) { minutes += seconds / 60; seconds %= 60; }
    if (minutes >= 60) { hours += minutes / 60; minutes %= 60; }
    if (hours >= 24) { hours %= 24; }
}

int Time::getHours() const { return hours; }
int Time::getMinutes() const { return minutes; }
int Time::getSeconds() const { return seconds; }

std::string Time::toString() const {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setw(2) << minutes;
    return ss.str();
}

std::string Time::toStringWithSeconds() const {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setw(2) << minutes
        << ":" << std::setw(2) << seconds;
    return ss.str();
}

int Time::toSeconds() const {
    return hours * 3600 + minutes * 60 + seconds;
}

void Time::addSeconds(int secs) {
    seconds += secs;
    if (seconds >= 60) { minutes += seconds / 60; seconds %= 60; }
    if (minutes >= 60) { hours += minutes / 60; minutes %= 60; }
    if (hours >= 24) { hours %= 24; }
}