#ifndef NODE_H
#define NODE_H

class NODE {
    int id;          // Changed from 'data' to 'id' to match your usage
    int quantity;
    NODE* nextPtr;
public:
    NODE(int x, int y) : id(x), quantity(y), nextPtr(nullptr) {}
    ~NODE() = default;
    
    // Setters
    void set_next(NODE* next) { nextPtr = next; }
    
    // Getters
    NODE* get_next() const { return nextPtr; }
    int get_value() const { return quantity; }
    int get_id() const { return id; }
    int get_quantity() const { return quantity; }
};

typedef NODE* NodePtr;

#endif