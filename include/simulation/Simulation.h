#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>
#include "SimulationEntry.h"
#include "include/simulation/StatisticManager.h"
#include "include/client/AbstractClient.h"

class Simulation {
private:
    SimulationEntry simulationEntry;
    StatisticManager statisticManager;

    int currentTime;
    std::vector<AbstractClient*> waitingQueue;
    std::vector<AbstractClient*> cashiers;

public:
    Simulation(const SimulationEntry& entry);

    void simulate();
    std::string simulationResults() const;

private:
    void generateClient();
    void processCashiers();
    void updateWaitingQueue();
};

#endif
