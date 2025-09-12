#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>
#include "simulation/StatisticManager.h"
#include "simulation/SimulationEntry.h"
#include "client/AbstractClient.hpp"

class Simulation {
private:
    SimulationEntry simulationEntry;
    StatisticManager statisticManager;

    int currentTime;
    std::vector<bank::client::AbstractClient*> waitingQueue;
    std::vector<bank::client::AbstractClient*> cashiers;

public:
    Simulation(const SimulationEntry& entry);

    void simulate();
    std::string simulationResults() const;
    const SimulationEntry& getEntry() const { return simulationEntry; }
    const StatisticManager& getStatisticManager() const { return statisticManager; }


private:
    void generateClient();
    void processCashiers();
    void updateWaitingQueue();
};

#endif
