#ifndef BANK_H
#define BANK_H

#include "Cashier.h"
#include <vector>

class Bank {
private:
    std::vector<Cashier> cashiers;

public:
    Bank(int numCashiers);

    void addCustomer(int customerId);
    void process();
};

#endif
