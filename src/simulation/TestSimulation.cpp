#include <iostream>
#include "include/simulation/Simulation.h"
#include "include/simulation/SimulationEntry.h"

int main() {
    const int CLIENT_ARRIVAL_INTERVAL = 5;
    const int MAX_SERVICE_TIME = 20;
    const int MIN_SERVICE_TIME = 10;
    const int CASHIER_COUNT = 3;
    const int SIMULATION_DURATION = 1000;
    const double PRIORITY_CLIENT_RATE = 0.1;
    const int CLIENT_PATIENCE_TIME = 7;

    SimulationEntry simulationEntry(
        SIMULATION_DURATION,
        CASHIER_COUNT,
        MIN_SERVICE_TIME,
        MAX_SERVICE_TIME,
        CLIENT_ARRIVAL_INTERVAL,
        PRIORITY_CLIENT_RATE,
        CLIENT_PATIENCE_TIME
    );

    Simulation simulation(simulationEntry);
    simulation.simulate();

    std::cout << simulation.simulationResults() << std::endl;

    return 0;
}
