#include "node.h"

Node::Node(const Request& value) : data(value), next(nullptr) {}

const Request& Node::getData() const { return data; }
Node* Node::getNext() const { return next; }
void Node::setData(const Request& value) { data = value; }
void Node::setNext(Node* node) { next = node; }