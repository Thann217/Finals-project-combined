#include "Queue.h"
#include <iostream>

Queue::Queue() : headPtr(nullptr), tailPtr(nullptr), size(0) {}

Queue::~Queue() {
    while (!isEmpty()) {
        dequeue();
    }
}

void Queue::enqueue(int x, int y, bool isUrgent) {
    NodePtr new_node = new NODE(x, y);
    if (isUrgent) {
        new_node->set_next(headPtr);
        headPtr = new_node;
        if (!tailPtr) tailPtr = headPtr;
    } else {
        if (tailPtr) tailPtr->set_next(new_node);
        else headPtr = new_node;
        tailPtr = new_node;
    }
    size++;
}

NodePtr Queue::dequeue() {
    if (isEmpty()) return nullptr;
    
    NodePtr temp = headPtr;
    headPtr = headPtr->get_next();
    if (!headPtr) tailPtr = nullptr;
    size--;
    
    return temp;
}

int Queue::getSize() const {
    return size;
}

bool Queue::isEmpty() const {
    return size == 0;
}

void Queue::displayRequests() const {
    NodePtr current = headPtr;
    while (current) {
        std::cout << "Request ID: " << current->get_id() 
                  << " | Quantity: " << current->get_quantity() << "kg\n";
        current = current->get_next();
    }
}