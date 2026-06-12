#include "lift.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

Lift::Lift(int floors)
    : totalFloors(floors), currentFloor(1), elapsedSeconds(0.0),
    isMoving(false), directionUp(true), isWaiting(true),
    requestCounter(0), liftSpeed(1.0), floorHeight(2.8),
    currentTime(0, 0, 0), lastPickCount(false), lastDropCount(false),
    lastPrintedFloor(-1) {  

    if (floors < 2)
        throw LogicException("Количество этажей должно быть не менее 2");

    srand(static_cast<unsigned>(time(nullptr)));
}

void Lift::updateCurrentTime() {
    currentTime = Time(0, 0, 0);
    currentTime.addSeconds(static_cast<int>(elapsedSeconds));
}

double Lift::getTravelTime() const {
    return floorHeight / liftSpeed;
}

double Lift::getStopTime() const {
    return BASE_STOP_TIME;
}

void Lift::sortRequests() {
    if (pendingRequests.isEmpty()) return;

    int count = pendingRequests.size();

    for (int i = 0; i < count - 1; i++) {
        Queue temp;
        Request prev;
        bool first = true;
        bool swapped = false;

        while (!pendingRequests.isEmpty()) {
            Request current = pendingRequests.front();
            pendingRequests.dequeue();

            if (first) {
                prev = current;
                first = false;
                continue;
            }

            if (prev.getCallTime().toSeconds() > current.getCallTime().toSeconds()) {
                temp.enqueue(current);
                swapped = true;
            }
            else {
                temp.enqueue(prev);
                prev = current;
            }
        }

        temp.enqueue(prev);

        while (!temp.isEmpty()) {
            pendingRequests.enqueue(temp.front());
            temp.dequeue();
        }

        if (!swapped) break;
    }
}

void Lift::addRequest(const Time& callTime, int fromFloor, int toFloor) {
    if (fromFloor < 1 || fromFloor > totalFloors ||
        toFloor < 1 || toFloor > totalFloors)
        throw InvalidInputException("Этаж должен быть от 1 до " + std::to_string(totalFloors));

    if (fromFloor == toFloor)
        throw InvalidInputException("Этаж посадки не может совпадать с этажом назначения");

    
    if (!isWaiting) {
        int callSec = callTime.toSeconds();
        int currentSec = currentTime.toSeconds();
        // Заявки с временем более чем на минуту раньше текущего — отклоняются
        if (!isWaiting && (currentTime.toSeconds() - callTime.toSeconds()) > 60) {
            throw InvalidInputException("Время заявки (" + callTime.toString() +
                ") уже прошло. Текущее время лифта: " + currentTime.toString());
        }
    }

    requestCounter++;
    Request req(requestCounter, callTime, fromFloor, toFloor);
    pendingRequests.enqueue(req);
}
void Lift::generateRandomRequests(int count, int spanMinutes, int startMinutes) {
    std::cout << "\nСоздаю " << count << " заявок..." << std::endl;
    for (int i = 0; i < count; i++) {
        int minutesOffset = rand() % (spanMinutes + 1);
        Time callTime(0, startMinutes + minutesOffset, 0);
        int from = 1 + (rand() % totalFloors);
        int to = 1 + (rand() % totalFloors);
        while (to == from) to = 1 + (rand() % totalFloors);
        requestCounter++;
        Request req(requestCounter, callTime, from, to);
        pendingRequests.enqueue(req);
    }
    std::cout << "Готово: " << count << " заявок" << std::endl;
}

int Lift::loadRequestsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw FileException("Не удалось открыть файл: " + filename);

    std::string line;
    int lineNum = 0;
    int loadedCount = 0;
    bool floorsRead = false;

    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        if (!floorsRead) {
            std::stringstream ss(line);
            int floors;
            if (ss >> floors) {
                if (floors < 2 || floors > 100) {
                    throw FileException("Некорректное количество этажей: " + std::to_string(floors));
                }
                totalFloors = floors;
                floorsRead = true;
                continue;
            }
            else {
                throw FileException("Ожидалось количество этажей, получено: " + line);
            }
        }

        std::stringstream ss(line);
        int hours, minutes, from, to;
        char colon;
        if (ss >> hours >> colon >> minutes >> from >> to) {
            try {
                Time callTime(hours, minutes, 0);
                addRequest(callTime, from, to);
                loadedCount++;
            }
            catch (const Exception& e) {
                std::cerr << " Строка " << lineNum << ": " << e.what() << std::endl;
            }
        }
        else {
            std::cerr << " Строка " << lineNum << ": неверный формат" << std::endl;
        }
    }
    file.close();
    return loadedCount;
}

void Lift::printStatus() const {
    std::string mode;
    if (isWaiting && pendingRequests.isEmpty()) mode = "ОЖИДАНИЕ";
    else if (isWaiting) mode = "ОЖИДАНИЕ (ждёт заявку)";
    else if (isMoving) mode = "ДВИЖЕНИЕ";
    else mode = "ОСТАНОВКА";

    std::cout << "\n==================================================" << std::endl;
    std::cout << "                ТЕКУЩИЙ СТАТУС ЛИФТА" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::left;
    std::cout << "  " << std::setw(14) << "Режим:" << mode << std::endl;
    std::cout << "  " << std::setw(14) << "Время:" << currentTime.toStringWithSeconds() << std::endl;
    std::cout << "  " << std::setw(14) << "Этаж:" << currentFloor << "/" << totalFloors << std::endl;
    std::cout << "  " << std::setw(14) << "Ожидают:" << pendingRequests.size() << " заяв." << std::endl;
    std::cout << "  " << std::setw(14) << "Обслужено:" << completedRequests.size() << " заяв." << std::endl;
    std::cout << "==================================================" << std::endl;
}

void Lift::printPassengersList() const {
    if (insidePassengers.isEmpty()) return;
    Queue temp = insidePassengers;
    int index = 1;
    while (!temp.isEmpty()) {
        Request req = temp.front(); temp.dequeue();
        std::cout << "    " << index << ". " << req.getFromFloor() << " -> " << req.getToFloor() << std::endl;
        index++;
    }
}

// ========== ТАБЛИЦА ==========
void Lift::printSimRow(const std::string& time, const std::string& mode,
    int floor, const std::string& direction,
    int inLift, const std::string& details,
    bool addSeparator) {

    // Добавляем пустую строку только если этаж изменился
    if (lastPrintedFloor != -1 && lastPrintedFloor != floor) {
        std::cout << std::endl;
    }

    // addSeparator больше не используем для пустых строк
    // if (addSeparator) std::cout << std::endl;  // Убираем эту строку

    std::cout << std::left;
    std::cout << " " << std::setw(10) << time;
    std::cout << " " << std::setw(14) << mode;
    std::cout << " " << std::setw(6) << floor;
    std::cout << " " << std::setw(8) << direction;
    std::cout << " " << std::setw(8) << inLift;
    std::cout << " " << details << std::endl;

    lastPrintedFloor = floor;
}

void Lift::printSimHeader() {
    std::cout << std::endl;
    std::cout << std::string(95, '—') << std::endl;
    std::cout << std::left;
    std::cout << " " << std::setw(10) << "ВРЕМЯ";
    std::cout << " " << std::setw(14) << "РЕЖИМ";
    std::cout << " " << std::setw(6) << "ЭТАЖ";
    std::cout << " " << std::setw(8) << "НАПР.";
    std::cout << " " << std::setw(8) << "В ЛИФТЕ";
    std::cout << " " << "ДЕТАЛИ" << std::endl;
    std::cout << std::string(95, '—') << std::endl;
}

void Lift::printSimFooter() {
    std::cout << std::string(95, '—') << std::endl;
}

bool Lift::needsStopHere() const {
    // Проверка 1: есть ли пассажиры, которым нужно выйти на этом этаже
    Queue temp = insidePassengers;
    while (!temp.isEmpty()) {
        if (temp.front().getToFloor() == currentFloor) return true;  // высадка — всегда остановка
        temp.dequeue();
    }

    // Если лифт заполнен — не останавливаемся для посадки
    if (insidePassengers.size() >= MAX_CAPACITY) return false;

    // Проверка 2: есть ли ожидающие заявки с этого этажа
    temp = pendingRequests;
    while (!temp.isEmpty()) {
        Request req = temp.front();
        temp.dequeue();
        if (req.getCallTime().toSeconds() <= currentTime.toSeconds() &&
            req.getFromFloor() == currentFloor) {
            if (insidePassengers.isEmpty() ||
                (directionUp && req.getToFloor() > currentFloor) ||
                (!directionUp && req.getToFloor() < currentFloor)) return true;
        }
    }
    return false;
}
void Lift::showStopIfNeeded() {
    if (isMoving) return;

    std::stringstream details;
    bool hasStop = false;

    if (lastDropCount) {
        details << lastDropDetails;
        hasStop = true;
    }
    if (lastPickCount) {
        if (hasStop) details << ", ";
        details << lastPickDetails;
        hasStop = true;
    }

    if (hasStop) {
        printSimRow(currentTime.toStringWithSeconds(), "ОСТАНОВКА", currentFloor, "—",
            static_cast<int>(insidePassengers.size()), details.str(), true);
        elapsedSeconds += getStopTime();
        updateCurrentTime();
        lastPickCount = false;
        lastDropCount = false;
    }
}

void Lift::runModeling() {
    if (pendingRequests.isEmpty()) {
        std::cout << "\nОШИБКА: Нет заявок для моделирования." << std::endl;
        return;
    }
    sortRequests();
    lastPrintedFloor = -1;

    std::cout << "\n======= МОДЕЛИРОВАНИЕ ЗАПУЩЕНО =======" << std::endl;
    std::cout << "  Скорость лифта: 1 м/с" << std::endl;
    std::cout << "  Высота этажа: 2.8 м" << std::endl;
    std::cout << "  Время на этаж: " << getTravelTime() << " с" << std::endl;
    std::cout << "  Время остановки: " << BASE_STOP_TIME << " с" << std::endl;
    std::cout << "  Максимальная вместимость лифта: " << MAX_CAPACITY << " заявок" << std::endl;

    std::cout << "\nРежимы лифта:" << std::endl;
    std::cout << "  ОТПРАВЛЕНИЕ   — движение к цели" << std::endl;
    std::cout << "  ДВИЖЕНИЕ      — проезд этажа" << std::endl;
    std::cout << "  ОСТАНОВКА     — посадка/высадка" << std::endl;
    std::cout << "  ОЖИДАНИЕ      — ожидание заявки" << std::endl;

    printSimHeader();

    while (!pendingRequests.isEmpty() || !insidePassengers.isEmpty()) {
        // Ожидание первой заявки
        if (isWaiting && !pendingRequests.isEmpty()) {
            Request firstReq = pendingRequests.front();
            int callSeconds = firstReq.getCallTime().toSeconds();
            int currentSeconds = currentTime.toSeconds();

            if (callSeconds > currentSeconds) {
                std::stringstream ss;
                ss << "ждёт до " << firstReq.getCallTime().toString()
                    << " (заявка #" << firstReq.getId() << ": "
                    << firstReq.getFromFloor() << "->" << firstReq.getToFloor() << ")";
                printSimRow(currentTime.toStringWithSeconds(), "ОЖИДАНИЕ", currentFloor, "—",
                    static_cast<int>(insidePassengers.size()), ss.str());
                elapsedSeconds += (callSeconds - currentSeconds);
                updateCurrentTime();
            }
            isWaiting = false;
        }

        processNewRequests();
        showStopIfNeeded();
        decideNextMove();

        if (isMoving) {
            while (isMoving) {
                double travelTime = getTravelTime();
                elapsedSeconds += travelTime;
                updateCurrentTime();
                if (directionUp) currentFloor++; else currentFloor--;

                bool stopNeeded = needsStopHere();
                std::string composition = formatComposition();

                printSimRow(currentTime.toStringWithSeconds(), "ДВИЖЕНИЕ", currentFloor,
                    directionUp ? "вверх" : "вниз",
                    static_cast<int>(insidePassengers.size()), composition);

                if (stopNeeded) {
                    isMoving = false;
                    disembarkPassengers();
                    processNewRequests();
                    showStopIfNeeded();
                    break;
                }
            }
        }
    }

    printSimFooter();
    std::cout << "\n======= МОДЕЛИРОВАНИЕ ЗАВЕРШЕНО =======" << std::endl;
    std::cout << "Финиш: " << currentTime.toStringWithSeconds() << ", этаж " << currentFloor << std::endl;
    std::cout << "Обслужено: " << completedRequests.size() << " заявок" << std::endl;
}

void Lift::printHistory() const {
    if (completedRequests.isEmpty()) {
        std::cout << "История пуста." << std::endl;
        return;
    }
    std::cout << "\n=== ПОРЯДОК ОБСЛУЖИВАНИЯ ===" << std::endl;
    completedRequests.print();
    std::cout << "==============================" << std::endl;
}

std::string Lift::formatPickDetails(Queue& pickedList) {
    std::stringstream ss;
    while (!pickedList.isEmpty()) {
        Request req = pickedList.front();
        pickedList.dequeue();
        ss << "+ заявка #" << req.getId() << "(" << req.getFromFloor() << "->" << req.getToFloor() << ")";
        if (!pickedList.isEmpty()) ss << ", ";
    }
    if (insidePassengers.size() >= MAX_CAPACITY) {
        ss << " (лифт заполнен!)";
    }
    return ss.str();
}

std::string Lift::formatDropDetails(Queue& leftList) {
    std::stringstream ss;
    while (!leftList.isEmpty()) {
        Request req = leftList.front();
        leftList.dequeue();
        ss << "- заявка #" << req.getId() << "(" << req.getFromFloor() << "->" << req.getToFloor() << ")";
        if (!leftList.isEmpty()) ss << ", ";
    }
    return ss.str();
}

std::string Lift::formatComposition() const {
    if (insidePassengers.isEmpty()) return "";
    std::stringstream ss;
    ss << "состав: ";
    Queue temp = insidePassengers;
    bool first = true;
    while (!temp.isEmpty()) {
        Request req = temp.front();
        temp.dequeue();
        if (!first) ss << ", ";
        ss << req.getFromFloor() << "->" << req.getToFloor();
        first = false;
    }
    return ss.str();
}
// ==================== ПРИВАТНЫЕ ====================
void Lift::processNewRequests() {
    if (pendingRequests.isEmpty()) { lastPickCount = false; return; }

    Queue tempQueue, pickedList;
    bool pickedSomeone = false;
    bool hasSameDirection = false;

    // Первый проход: собираем попутные заявки (приоритет)
    while (!pendingRequests.isEmpty()) {
        Request req = pendingRequests.front();
        pendingRequests.dequeue();

        if (insidePassengers.size() >= MAX_CAPACITY) {
            tempQueue.enqueue(req);
            continue;
        }

        if (req.getCallTime().toSeconds() <= currentTime.toSeconds() &&
            req.getFromFloor() == currentFloor && !isMoving) {

            // Лифт пуст — берём всех
            if (insidePassengers.isEmpty()) {
                insidePassengers.enqueue(req);
                pickedList.enqueue(req);
                pickedSomeone = true;
                continue;
            }

            // Есть высадка — приоритет попутным
            if (lastDropCount) {
                bool isSameDirection = (directionUp && req.getToFloor() > currentFloor) ||
                    (!directionUp && req.getToFloor() < currentFloor);
                if (isSameDirection) {
                    insidePassengers.enqueue(req);
                    pickedList.enqueue(req);
                    pickedSomeone = true;
                    hasSameDirection = true;
                    continue;
                }
                // Не попутная — откладываем
                tempQueue.enqueue(req);
                continue;
            }

            // Нет высадки — только попутные
            if (!lastDropCount) {
                bool isSameDirection = (directionUp && req.getToFloor() > currentFloor) ||
                    (!directionUp && req.getToFloor() < currentFloor);
                if (isSameDirection) {
                    insidePassengers.enqueue(req);
                    pickedList.enqueue(req);
                    pickedSomeone = true;
                    continue;
                }
            }
        }
        tempQueue.enqueue(req);
    }

    // Второй проход: если была высадка, но попутных нет — берём любого
    if (lastDropCount && !hasSameDirection && !tempQueue.isEmpty()) {
        Queue remaining;
        while (!tempQueue.isEmpty()) {
            Request req = tempQueue.front();
            tempQueue.dequeue();
            if (insidePassengers.size() < MAX_CAPACITY &&
                req.getCallTime().toSeconds() <= currentTime.toSeconds() &&
                req.getFromFloor() == currentFloor && !isMoving) {
                insidePassengers.enqueue(req);
                pickedList.enqueue(req);
                pickedSomeone = true;
                break;  // берём одного
            }
            remaining.enqueue(req);
        }
        while (!remaining.isEmpty()) {
            tempQueue.enqueue(remaining.front());
            remaining.dequeue();
        }
    }

    // Возвращаем необработанные заявки обратно в очередь
    while (!tempQueue.isEmpty()) {
        pendingRequests.enqueue(tempQueue.front());
        tempQueue.dequeue();
    }

    // Формируем строку для вывода
    if (pickedSomeone) {
        lastPickDetails = formatPickDetails(pickedList);
        lastPickCount = true;
    }
    else {
        lastPickCount = false;
    }
}
void Lift::disembarkPassengers() {
    if (insidePassengers.isEmpty()) { lastDropCount = false; return; }

    Queue tempQueue, leftList;
    bool disembarked = false;

    while (!insidePassengers.isEmpty()) {
        Request req = insidePassengers.front(); insidePassengers.dequeue();
        if (req.getToFloor() == currentFloor) {
            completedRequests.enqueue(req);
            leftList.enqueue(req);
            disembarked = true;
        }
        else { tempQueue.enqueue(req); }
    }

    while (!tempQueue.isEmpty()) { insidePassengers.enqueue(tempQueue.front()); tempQueue.dequeue(); }

    if (disembarked) {
        lastDropDetails = formatDropDetails(leftList);
        lastDropCount = true;
    }
    else { lastDropCount = false; }
}
void Lift::decideNextMove() {
    if (insidePassengers.isEmpty() && pendingRequests.isEmpty()) {
        isMoving = false;
        return;
    }

    // Приоритет 1: Если есть пассажиры внутри - едем к их этажам назначения
    if (!insidePassengers.isEmpty()) {
        Request first = insidePassengers.front();
        directionUp = (first.getToFloor() > currentFloor);
        isMoving = true;
        std::stringstream ss;
        ss << "к этажу " << first.getToFloor() << " (высадка)";
        printSimRow(currentTime.toStringWithSeconds(), "ОТПРАВЛЕНИЕ", currentFloor,
            directionUp ? "вверх" : "вниз",
            static_cast<int>(insidePassengers.size()), ss.str());
        return;
    }

    // Приоритет 2: Лифт пустой - ищем заявку
    if (!pendingRequests.isEmpty()) {
        // Находим самую раннюю заявку по времени
        Request best = pendingRequests.front();
        int earliestTime = best.getCallTime().toSeconds();

        // Собираем все заявки с одинаковым (самым ранним) временем
        Queue candidates;
        Queue temp = pendingRequests;
        while (!temp.isEmpty()) {
            Request current = temp.front();
            temp.dequeue();
            if (current.getCallTime().toSeconds() == earliestTime) {
                candidates.enqueue(current);
            }
        }

        // Среди кандидатов ищем ближайшую по расстоянию
        best = candidates.front();
        candidates.dequeue();
        int bestDist = abs(best.getFromFloor() - currentFloor);

        while (!candidates.isEmpty()) {
            Request current = candidates.front();
            candidates.dequeue();
            int currentDist = abs(current.getFromFloor() - currentFloor);

            if (currentDist < bestDist) {
                best = current;
                bestDist = currentDist;
            }
            // Если расстояние одинаковое - предпочитаем заявку в текущем направлении
            else if (currentDist == bestDist) {
                bool currentSameDir = (directionUp && current.getFromFloor() > currentFloor) ||
                    (!directionUp && current.getFromFloor() < currentFloor);
                bool bestSameDir = (directionUp && best.getFromFloor() > currentFloor) ||
                    (!directionUp && best.getFromFloor() < currentFloor);

                if (currentSameDir && !bestSameDir) {
                    best = current;
                }
            }
        }

        int callSeconds = best.getCallTime().toSeconds();
        int currentSeconds = currentTime.toSeconds();

        // Если заявка ещё не наступила - ждём
        if (callSeconds > currentSeconds) {
            std::stringstream ss;
            ss << "ждёт до " << best.getCallTime().toString()
                << " (заявка #" << best.getId() << ": "
                << best.getFromFloor() << "->" << best.getToFloor() << ")";
            printSimRow(currentTime.toStringWithSeconds(), "ОЖИДАНИЕ", currentFloor, "—",
                static_cast<int>(insidePassengers.size()), ss.str());
            elapsedSeconds += (callSeconds - currentSeconds);
            updateCurrentTime();
            isMoving = false;
            return;
        }
        

        // Определяем направление к выбранной заявке
        if (best.getFromFloor() > currentFloor) {
            directionUp = true;
            isMoving = true;
        }
        else if (best.getFromFloor() < currentFloor) {
            directionUp = false;
            isMoving = true;
        }
        else {
            isMoving = false;
            return;
        }

        std::stringstream ss;
        ss << "к этажу " << best.getFromFloor() << " за пассажиром #" << best.getId();
        printSimRow(currentTime.toStringWithSeconds(), "ОТПРАВЛЕНИЕ", currentFloor,
            directionUp ? "вверх" : "вниз",
            static_cast<int>(insidePassengers.size()), ss.str());
    }
}

void Lift::printPendingRequests() const {
    if (pendingRequests.isEmpty()) {
        std::cout << "  (нет ожидающих заявок)" << std::endl;
        return;
    }
    std::cout << "\n--- Список введённых заявок ---" << std::endl;
    pendingRequests.print();
    std::cout << "--------------------------------" << std::endl;
}