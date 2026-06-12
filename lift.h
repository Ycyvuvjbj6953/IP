#ifndef LIFT_H
#define LIFT_H
#include "request.h"
#include "queue.h"
#include <string>
class Lift {
private:
    int totalFloors;
    int currentFloor;
    double elapsedSeconds;
    bool isMoving;
    bool directionUp;
    bool isWaiting;
    int requestCounter;
    int lastPrintedFloor;
    double liftSpeed;
    double floorHeight;
    Time currentTime;
    Queue pendingRequests;
    Queue insidePassengers;
    Queue completedRequests;
    std::string lastPickDetails;
    std::string lastDropDetails;
    bool lastPickCount;
    bool lastDropCount;
    static constexpr double BASE_STOP_TIME = 10.0;
    static constexpr int MAX_CAPACITY = 10;
    double getTravelTime() const;
    double getStopTime() const;
    bool needsStopHere() const;
    void sortRequests();
    void processNewRequests();
    void disembarkPassengers();
    void decideNextMove();
    void updateCurrentTime();
    void printPassengersList() const;
    static void printSimHeader();
    static void printSimFooter();
    void printSimRow(const std::string& time, const std::string& mode,
        int floor, const std::string& direction,
        int inLift, const std::string& details,
        bool addSeparator = false);
    std::string formatPickDetails(Queue& pickedList);
    std::string formatDropDetails(Queue& leftList);
    std::string formatComposition() const;
    void showStopIfNeeded();

public:
    Lift(int floors);
    void printPendingRequests() const;
    void addRequest(const Time& callTime, int fromFloor, int toFloor);
    void runModeling();
    void printHistory() const;
    void printStatus() const;
    void generateRandomRequests(int count, int spanMinutes, int startMinutes = 0);
    int loadRequestsFromFile(const std::string& filename);  // теперь возвращает int

    int getTotalFloors() const { return totalFloors; }
};

#endif