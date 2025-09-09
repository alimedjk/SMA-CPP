#include "Bank.h"

Bank::Bank(int numCashiers) {
    for (int i = 0; i < numCashiers; i++) {
        cashiers.emplace_back(i + 1);
    }
}

void Bank::addCustomer(int customerId) {
    int index = customerId % cashiers.size();
    cashiers[index].addCustomer(customerId);
}

void Bank::process() {
    for (auto &cashier : cashiers) {
        cashier.serveCustomer();
    }
}
