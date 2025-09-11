#include "bank/Queue.h"
#include <stdexcept>

void Queue::enqueue(int value) {
    q.push(value);
}

int Queue::dequeue() {
    if (q.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    int front = q.front();
    q.pop();
    return front;
}

bool Queue::isEmpty() const {
    return q.empty();
}

int Queue::size() const {
    return q.size();
}
