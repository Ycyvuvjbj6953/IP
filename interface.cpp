#include "interface.h"
#include "validator.h"
#include <iostream>
#include <limits>
#include <fstream>

Interface::Interface()
    : modeling(2), configured(false), floors(0) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       МОДЕЛИРОВАНИЕ РАБОТЫ ЛИФТА" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void Interface::run() {
    bool isRunning = true;
    while (isRunning) {
        displayMainMenu();
        int choice = Validator::getValidatedInt("Выберите пункт: ", 1, 7);  

        switch (choice) {
        case 1: handleAddRequests(); break;
        case 2:
            if (!configured)
                std::cerr << "\nОШИБКА: Сначала добавьте заявки (пункт 1)." << std::endl;
            else
                modeling.runModeling();
            break;
        case 3:
            if (!configured)
                std::cerr << "\nОШИБКА: Сначала добавьте заявки (пункт 1)." << std::endl;
            else
                modeling.printHistory();
            break;
        case 4:
            if (!configured)
                std::cerr << "\nОШИБКА: Сначала добавьте заявки (пункт 1)." << std::endl;
            else
                modeling.printStatus();
            break;
        case 5:
            handleReset();
            break;
        case 6:
            demonstrateExceptions();  // Демонстрация исключений
            break;
        case 7:
            std::cout << "\nДо свидания!" << std::endl;
            isRunning = false;
            break;
        }
    }
}

void Interface::displayMainMenu() const {
    std::cout << "\n========= ГЛАВНОЕ МЕНЮ =========" << std::endl;
    std::cout << "1. Добавить заявки" << std::endl;
    std::cout << "2. Запустить моделирование" << std::endl;
    std::cout << "3. Порядок обслуживания" << std::endl;
    std::cout << "4. Текущий статус лифта" << std::endl;
    std::cout << "5. Сброс" << std::endl;
    std::cout << "6. Демонстрация исключений" << std::endl;
    std::cout << "7. Выход" << std::endl;
    std::cout << "================================" << std::endl;
}

void Interface::displayInputSubmenu() const {
    std::cout << "\n--- Выбор способа ввода заявок ---" << std::endl;
    std::cout << "1. С клавиатуры" << std::endl;
    std::cout << "2. Из файла" << std::endl;
    std::cout << "3. Автоматическая генерация" << std::endl;
    std::cout << "----------------------------------" << std::endl;
}

void Interface::handleAddRequests() {
    displayInputSubmenu();
    int inputChoice = Validator::getValidatedInt("Выберите способ ввода: ", 1, 3);

    switch (inputChoice) {
    case 1: handleKeyboardInput(); break;
    case 2: handleFileInput(); break;
    case 3: handleGenerateInput(); break;
    }
}

void Interface::handleKeyboardInput() {
    if (!configured) {
        floors = Validator::getValidatedInt("Введите количество этажей в здании: ", 1, 100);

        try {
            modeling = Lift(floors);
            configured = true;
            std::cout << "\nСоздан лифт в здании на " << floors << " этажей.\n" << std::endl;
        }
        catch (const LogicException& e) {
            std::cerr << "\nОШИБКА: " << e.what() << std::endl;
            floors = 0;
            return;  // Возвращаемся в меню, лифт не создан
        }
    }

    std::cout << "--- Ручной ввод заявок ---" << std::endl;
    int count = Validator::getValidatedInt("Сколько заявок добавить (1-100): ", 1, 100);
    std::cout << "\nВводите заявки по одной.\n" << std::endl;

    int added = 0;
    for (int i = 0; i < count; i++) {
        std::cout << "Заявка #" << (i + 1) << " из " << count << ":" << std::endl;
        std::cout << "  Время вызова:" << std::endl;
        Time callTime = Validator::inputTimeHM();
        int from = Validator::getValidatedInt("  Этаж посадки: ", 1, floors);
        int to = Validator::getValidatedInt("  Этаж назначения: ", 1, floors);

        while (to == from) {
            std::cerr << "  ОШИБКА: Этаж назначения не может совпадать с этажом посадки." << std::endl;
            to = Validator::getValidatedInt("  Этаж назначения: ", 1, floors);
        }

        try {
            modeling.addRequest(callTime, from, to);
            added++;
        }
        catch (const Exception& e) {
            std::cerr << "  ОШИБКА: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "Ввод завершён. Добавлено заявок: " << added << std::endl;
    modeling.printPendingRequests();
}

void Interface::handleFileInput() {
    std::cout << "\n--- Загрузка из файла ---" << std::endl;

    std::string filename;
    std::cout << "Имя файла: ";
    std::getline(std::cin, filename);

    std::ifstream testFile(filename);
    if (!testFile.is_open()) {
        std::cerr << "\nОШИБКА: Файл не найден: " << filename << std::endl;
        return;
    }
    testFile.close();

    try {
        int oldFloors = configured ? floors : -1;
        int loaded = modeling.loadRequestsFromFile(filename);
        int fileFloors = modeling.getTotalFloors();

        if (configured && oldFloors != fileFloors) {
            std::cerr << "\nОШИБКА: Количество этажей в файле (" << fileFloors
                << ") не совпадает с заданным (" << oldFloors << ")." << std::endl;
            modeling = Lift(oldFloors);
            return;
        }

        configured = true;
        floors = fileFloors;
        std::cout << "\nЗаявки успешно загружены. Добавлено: " << loaded << ".\n" << std::endl;
        modeling.printPendingRequests();
    }
    catch (const FileException& e) {
        std::cerr << "ОШИБКА: " << e.what() << std::endl;
    }
}

void Interface::handleGenerateInput() {
    if (!configured) {
        floors = Validator::getValidatedInt("Введите количество этажей в здании: ", 1, 100);

        try {
            modeling = Lift(floors);
            configured = true;
            std::cout << "\nСоздан лифт в здании на " << floors << " этажей.\n" << std::endl;
        }
        catch (const LogicException& e) {
            std::cerr << "\nОШИБКА: " << e.what() << std::endl;
            floors = 0;
            return;
        }
    }

    std::cout << "--- Автоматическая генерация заявок ---" << std::endl;
    int count = Validator::getValidatedInt("Сколько заявок сгенерировать (1-100): ", 1, 100);

    std::cout << "Интервал времени для заявок:" << std::endl;
    int fromHour = Validator::getValidatedInt("  От (часы, 0-23): ", 0, 23);
    int fromMin = Validator::getValidatedInt("  От (минуты, 0-59): ", 0, 59);
    int toHour = Validator::getValidatedInt("  До (часы, 0-23): ", 0, 23);
    int toMin = Validator::getValidatedInt("  До (минуты, 0-59): ", 0, 59);

    int span = (toHour * 60 + toMin) - (fromHour * 60 + fromMin);
    if (span <= 0) {
        std::cerr << "ОШИБКА: Время «до» должно быть больше времени «от»." << std::endl;
        return;
    }

    modeling.generateRandomRequests(count, span, fromHour * 60 + fromMin);
    modeling.printPendingRequests();
}

void Interface::handleReset() {
    modeling = Lift(2);
    configured = false;
    floors = 0;
    std::cout << "\nВсе данные очищены." << std::endl;
}

void Interface::demonstrateExceptions() {
    std::cout << "\n======= ДЕМОНСТРАЦИЯ РАБОТЫ ОЧЕРЕДИ И ИСКЛЮЧЕНИЙ =======" << std::endl;
    std::cout << "1. Работа с очередью (добавление, удаление, просмотр)" << std::endl;
    std::cout << "2. Исключения при работе с пустой очередью" << std::endl;
    std::cout << "3. Исключения при неверных данных" << std::endl;
    std::cout << "========================================================" << std::endl;

    int choice = Validator::getValidatedInt("Выберите демонстрацию: ", 1, 3);

    switch (choice) {
    case 1: {
        std::cout << "\n=== ДЕМОНСТРАЦИЯ РАБОТЫ ОЧЕРЕДИ ===" << std::endl;

        Queue demoQueue;

        // 1. Показываем пустую очередь
        std::cout << "\n1. Создана пустая очередь:" << std::endl;
        demoQueue.print();
        std::cout << "Размер очереди: " << demoQueue.size() << std::endl;

        // 2. Добавляем заявки
        std::cout << "\n2. Добавление заявок в очередь:" << std::endl;
        Time time1(10, 30, 0);
        Time time2(10, 35, 0);
        Time time3(10, 40, 0);

        Request req1(1, time1, 2, 5);
        Request req2(2, time2, 1, 4);
        Request req3(3, time3, 3, 6);

        demoQueue.enqueue(req1);
        demoQueue.enqueue(req2);
        demoQueue.enqueue(req3);

        std::cout << "Добавлены заявки:" << std::endl;
        demoQueue.print();
        std::cout << "Размер очереди: " << demoQueue.size() << std::endl;

        // 3. Просмотр первого элемента
        std::cout << "\n3. Просмотр первого элемента (front):" << std::endl;
        Request first = demoQueue.front();
        std::cout << first.toString() << std::endl;

        // 4. Удаление из начала очереди
        std::cout << "\n4. Удаление первого элемента (dequeue):" << std::endl;
        demoQueue.dequeue();
        std::cout << "Очередь после удаления:" << std::endl;
        demoQueue.print();
        std::cout << "Размер очереди: " << demoQueue.size() << std::endl;

        // 5. Копирование очереди
        std::cout << "\n5. Копирование очереди:" << std::endl;
        Queue copyQueue = demoQueue;
        std::cout << "Оригинал: " << demoQueue.size() << " заявок" << std::endl;
        std::cout << "Копия: " << copyQueue.size() << " заявок" << std::endl;
        std::cout << "Содержимое копии:" << std::endl;
        copyQueue.print();

        // 6. Присваивание очереди
        std::cout << "\n6. Присваивание очереди:" << std::endl;
        Queue assignedQueue;
        assignedQueue = demoQueue;
        std::cout << "Присвоенная очередь: " << assignedQueue.size() << " заявок" << std::endl;
        assignedQueue.print();

        // 7. Очистка очереди
        std::cout << "\n7. Очистка очереди (удаление всех элементов):" << std::endl;
        while (!demoQueue.isEmpty()) {
            std::cout << "Удаляю: " << demoQueue.front().toString() << std::endl;
            demoQueue.dequeue();
        }
        std::cout << "Очередь после очистки:" << std::endl;
        demoQueue.print();
        std::cout << "Размер очереди: " << demoQueue.size() << std::endl;

        break;
    }

    case 2: {
        std::cout << "\n=== ИСКЛЮЧЕНИЯ ПРИ РАБОТЕ С ПУСТОЙ ОЧЕРЕДЬЮ ===" << std::endl;

        Queue emptyQueue;

        std::cout << "\nСоздана пустая очередь:" << std::endl;
        emptyQueue.print();

        // 1. Попытка удаления из пустой очереди
        std::cout << "\n1. Попытка dequeue() из пустой очереди:" << std::endl;
        try {
            emptyQueue.dequeue();
        }
        catch (const DataStructureException& e) {
            std::cerr  << e.what() << std::endl;
        }

        // 2. Попытка доступа к первому элементу
        std::cout << "\n2. Попытка front() из пустой очереди:" << std::endl;
        try {
            emptyQueue.front();
        }
        catch (const DataStructureException& e) {
            std::cerr  << e.what() << std::endl;
        }

        // Показываем, что очередь осталась пустой
        std::cout << "\nОчередь после попыток:" << std::endl;
        emptyQueue.print();

        break;
    }

    case 3: {
        std::cout << "\n=== ИСКЛЮЧЕНИЯ ПРИ НЕВЕРНЫХ ДАННЫХ ===" << std::endl;

        // Создаём лифт для демонстрации
        if (!configured) {
            floors = 5;
            modeling = Lift(floors);
            configured = true;
        }

        Time time(10, 30, 0);

        // 1. Неверный этаж
        std::cout << "\n1. Добавление заявки с этажом 0:" << std::endl;
        std::cout << "   Данные: время=" << time.toString() << ", с 0 на 5 этаж" << std::endl;
        try {
            modeling.addRequest(time, 0, 5);
        }
        catch (const InvalidInputException& e) {
            std::cerr << e.what() << std::endl;
        }

        // 2. Одинаковые этажи
        std::cout << "\n2. Добавление заявки с одинаковыми этажами:" << std::endl;
        std::cout << "   Данные: время=" << time.toString() << ", с 3 на 3 этаж" << std::endl;
        try {
            modeling.addRequest(time, 3, 3);
        }
        catch (const InvalidInputException& e) {
            std::cerr  << e.what() << std::endl;
        }

        // 3. Попытка создать лифт с 1 этажом
        std::cout << "\n3. Создание лифта с 1 этажом:" << std::endl;
        try {
            Lift badLift(1);
        }
        catch (const LogicException& e) {
            std::cerr  << e.what() << std::endl;
        }

        // 4. Попытка загрузить несуществующий файл
        std::cout << "\n4. Загрузка несуществующего файла:" << std::endl;
        std::cout << "   Файл: nonexistent.txt" << std::endl;
        try {
            modeling.loadRequestsFromFile("nonexistent.txt");
        }
        catch (const FileException& e) {
            std::cerr << e.what() << std::endl;
        }

        // Показываем текущее состояние
        std::cout << "\nТекущие заявки после попыток:" << std::endl;
        modeling.printPendingRequests();

        break;
    }
    }

    std::cout << "\n======= ДЕМОНСТРАЦИЯ ЗАВЕРШЕНА =======" << std::endl;
}