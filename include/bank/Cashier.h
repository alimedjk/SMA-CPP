#ifndef CASHIER_H
#define CASHIER_H

#include "/include/bank/Queue.h"

class Cashier {
private:
    int id;
    bool busy;
    int serviceTime;
    Queue queue;

public:
    Cashier(int cashierId);

    void addCustomer(int customerId);
    void serveCustomer();
    bool isBusy() const;
    int getId() const;
};

#endif
