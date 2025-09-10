#ifndef STATISTIC_MANAGER_H
#define STATISTIC_MANAGER_H

#include <vector>
#include "inculde/client/AbstractClient.hpp"

class StatisticManager {
private:
    std::vector<AbstractClient*> servedClients;
    std::vector<AbstractClient*> nonServedClients;

    int simulationDuration = 0;
    int occupiedCashier = 0;

public:
    void registerServedClient(AbstractClient* client);
    void registerNonServedClient(AbstractClient* client);
    void simulationDurationRecord();
    void cashierOccupationRecord();

    double calculateAverageCashierOccupationRate(int cashierCount) const;
    double calculateAverageClientWaitingTime() const;
    double calculateAverageClientServiceTime() const;
    double calculateClientSatisfactionRate() const;

    int servedClientCount() const;
    int nonServedClientCount() const;
};

#endif
