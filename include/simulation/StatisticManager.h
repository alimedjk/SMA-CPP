#ifndef STATISTIC_MANAGER_H
#define STATISTIC_MANAGER_H

#include <vector>
#include "client/AbstractClient.hpp"
namespace bank::client { class AbstractClient; }
class StatisticManager {
private:
    std::vector<bank::client::AbstractClient*> servedClients;
    std::vector<bank::client::AbstractClient*> nonServedClients;

    int simulationDuration = 0;
    int occupiedCashier = 0;

public:
    void registerServedClient(bank::client::AbstractClient* client);
    void registerNonServedClient(bank::client::AbstractClient* client);
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
