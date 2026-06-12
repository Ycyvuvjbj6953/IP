#include "queue.h"
#include <iostream>

Queue::Queue() : m_front(nullptr), m_rear(nullptr), m_count(0) {}

Queue::~Queue() {
    while (!isEmpty()) dequeue();
}

Queue::Queue(const Queue& other)
    : m_front(nullptr), m_rear(nullptr), m_count(0) {
    Node* current = other.m_front;
    while (current) { enqueue(current->getData()); current = current->getNext(); }
}

Queue& Queue::operator=(const Queue& other) {
    if (this != &other) {
        while (!isEmpty()) dequeue();
        Node* current = other.m_front;
        while (current) { enqueue(current->getData()); current = current->getNext(); }
    }
    return *this;
}

void Queue::enqueue(const Request& value) {
    Node* newNode = new Node(value);
    if (isEmpty()) { m_front = m_rear = newNode; }
    else { m_rear->setNext(newNode); m_rear = newNode; }
    m_count++;
}

void Queue::dequeue() {
    if (isEmpty()) throw DataStructureException("Попытка удаления из пустой очереди");
    Node* temp = m_front;
    m_front = m_front->getNext();
    delete temp;
    m_count--;
    if (!m_front) m_rear = nullptr;
}

const Request& Queue::front() const {
    if (isEmpty()) throw DataStructureException("Попытка доступа к первому элементу пустой очереди");
    return m_front->getData();
}

bool Queue::isEmpty() const { return m_front == nullptr; }
size_t Queue::size() const { return m_count; }

void Queue::print() const {
    Node* current = m_front;
    int index = 1;
    while (current) {
        Request req = current->getData();
        std::cout << index << ". Заявка #" << req.getId() << ": "
            << req.getFromFloor() << " -> " << req.getToFloor()
            << "  (" << req.getCallTime().toString() << ")" << std::endl;
        current = current->getNext();
        index++;
    }
}