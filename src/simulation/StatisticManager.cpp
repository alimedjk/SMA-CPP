#include "simulation/StatisticManager.h"
#include "client/AbstractClient.hpp"
using bank::client::AbstractClient;

void StatisticManager::registerServedClient(AbstractClient* c){ servedClients.push_back(c); }
void StatisticManager::registerNonServedClient(AbstractClient* c){ nonServedClients.push_back(c); }
void StatisticManager::simulationDurationRecord(){ simulationDuration++; }
void StatisticManager::cashierOccupationRecord(){ occupiedCashier++; }

double StatisticManager::calculateAverageCashierOccupationRate(int cashierCount) const {
    if (simulationDuration == 0 || cashierCount == 0) return 0.0;
    return (occupiedCashier * 100.0 / simulationDuration) / cashierCount;
}
double StatisticManager::calculateAverageClientWaitingTime() const {
    if (servedClients.empty()) return 0.0;
    int total = 0;
    for (auto* c : servedClients) total += c->serviceStartTime() - c->arrivalTime();
    return double(total) / servedClients.size();
}
double StatisticManager::calculateAverageClientServiceTime() const {
    if (servedClients.empty()) return 0.0;
    int total = 0;
    for (auto* c : servedClients) total += c->departureTime() - c->serviceStartTime();
    return double(total) / servedClients.size();
}
int  StatisticManager::servedClientCount() const { return (int)servedClients.size(); }
int  StatisticManager::nonServedClientCount() const { return (int)nonServedClients.size(); }
double StatisticManager::calculateClientSatisfactionRate() const {
    int tot = (int)(servedClients.size() + nonServedClients.size());
    return tot ? servedClients.size() * 100.0 / tot : 0.0;
}
