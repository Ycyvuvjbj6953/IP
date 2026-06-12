#ifndef QUEUE_H
#define QUEUE_H

#include "exceptions.h"
#include "node.h"

class Queue {
private:
    Node* m_front;
    Node* m_rear;
    size_t m_count;

public:
    Queue();
    ~Queue();
    Queue(const Queue& other);
    Queue& operator=(const Queue& other);

    void enqueue(const Request& value);
    void dequeue();
    const Request& front() const;  // גלוסעמ Request front() const;
    bool isEmpty() const;
    size_t size() const;
    void print() const;
};

#endif