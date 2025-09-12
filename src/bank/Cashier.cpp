#include "bank/Cashier.h"
#include <iostream>

Cashier::Cashier(int cashierId) : id(cashierId), busy(false), serviceTime(0) {}

void Cashier::addCustomer(int customerId) {
    queue.enqueue(customerId);
}

void Cashier::serveCustomer() {
    if (!queue.isEmpty()) {
        int customer = queue.dequeue();
        busy = true;
        serviceTime++;
        std::cout << "Cashier " << id << " is serving customer " << customer << std::endl;
    } else {
        busy = false;
    }
}

bool Cashier::isBusy() const {
    return busy;
}

int Cashier::getId() const {
    return id;
}
