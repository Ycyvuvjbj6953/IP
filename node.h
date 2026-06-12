#pragma once
#ifndef NODE_H
#define NODE_H

#include "request.h"

class Node {
private:
    Request data;
    Node* next;

public:
    Node(const Request& value);
    const Request& getData() const;  // גלוסעמ Request getData() const;
    Node* getNext() const;
    void setData(const Request& value);
    void setNext(Node* node);
};

#endif