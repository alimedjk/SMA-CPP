#include "StatisticManager.h"

void StatisticManager::registerServedClient(AbstractClient* client) {
    servedClients.push_back(client);
}

void StatisticManager::registerNonServedClient(AbstractClient* client) {
    nonServedClients.push_back(client);
}

void StatisticManager::simulationDurationRecord() {
    simulationDuration++;
}

void StatisticManager::cashierOccupationRecord() {
    occupiedCashier++;
}

double StatisticManager::calculateAverageCashierOccupationRate(int cashierCount) const {
    if (simulationDuration == 0 || cashierCount == 0) return 0.0;
    return (occupiedCashier * 100.0 / simulationDuration) / cashierCount;
}

double StatisticManager::calculateAverageClientWaitingTime() const {
    if (servedClients.empty()) return 0.0;
    int totalWaitingTime = 0;
    for (auto client : servedClients) {
        totalWaitingTime += client->getServiceStartTime() - client->getArrivalTime();
    }
    return static_cast<double>(totalWaitingTime) / servedClients.size();
}

double StatisticManager::calculateAverageClientServiceTime() const {
    if (servedClients.empty()) return 0.0;
    int totalServiceTime = 0;
    for (auto client : servedClients) {
        totalServiceTime += client->getDepartureTime() - client->getServiceStartTime();
    }
    return static_cast<double>(totalServiceTime) / servedClients.size();
}

int StatisticManager::servedClientCount() const {
    return servedClients.size();
}

int StatisticManager::nonServedClientCount() const {
    return nonServedClients.size();
}

double StatisticManager::calculateClientSatisfactionRate() const {
    int total = servedClients.size() + nonServedClients.size();
    if (total == 0) return 0.0;
    return servedClients.size() * 100.0 / total;
}
