#ifndef QUEUE_H
#define QUEUE_H

#include "Node.h"

class Queue {
private:
    NodePtr headPtr, tailPtr;
    int size;

public:
    Queue();
    ~Queue();
    
    // Corrected method signatures:
    void enqueue(int x, int y, bool isUrgent = false);
    NodePtr dequeue();  // Changed from dequeue2
    int getSize() const;  // Changed from get_size
    bool isEmpty() const;
    void displayRequests() const;
};

#endif