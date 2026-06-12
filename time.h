#ifndef TIMEHM_H
#define TIMEHM_H
#include <string>

class Time {
private:
    int hours;
    int minutes;
    int seconds;
public:
    Time();
    Time(int h, int m, int s = 0);
    int getHours() const;
    int getMinutes() const;
    int getSeconds() const;
    std::string toString() const;
    std::string toStringWithSeconds() const;
    int toSeconds() const;
    void addSeconds(int secs);
};
#endif