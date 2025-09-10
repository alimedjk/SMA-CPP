#include "include/simulation/SimulationEntry.h"

SimulationEntry::SimulationEntry(int simulationDuration,
                                 int cashierCount,
                                 int minServiceTime,
                                 int maxServiceTime,
                                 int clientArrivalInterval,
                                 double priorityClientRate,
                                 int clientPatienceTime)
    : simulationDuration(simulationDuration),
      cashierCount(cashierCount),
      minServiceTime(minServiceTime),
      maxServiceTime(maxServiceTime),
      clientArrivalInterval(clientArrivalInterval),
      priorityClientRate(priorityClientRate),
      clientPatienceTime(clientPatienceTime) {}

int SimulationEntry::getSimulationDuration() const { return simulationDuration; }
int SimulationEntry::getCashierCount() const { return cashierCount; }
int SimulationEntry::getMinServiceTime() const { return minServiceTime; }
int SimulationEntry::getMaxServiceTime() const { return maxServiceTime; }
int SimulationEntry::getClientArrivalInterval() const { return clientArrivalInterval; }
double SimulationEntry::getPriorityClientRate() const { return priorityClientRate; }
int SimulationEntry::getClientPatienceTime() const { return clientPatienceTime; }
