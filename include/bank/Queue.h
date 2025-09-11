#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
class Queue {
private:
    std::queue<int> q;

public:
    void enqueue(int value);
    int dequeue();
    bool isEmpty() const;
    int size() const;
};

#endif
