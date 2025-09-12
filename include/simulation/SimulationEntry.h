#ifndef SIMULATION_ENTRY_H
#define SIMULATION_ENTRY_H

class SimulationEntry {
private:
    int simulationDuration;
    int cashierCount;
    int minServiceTime;
    int maxServiceTime;
    int clientArrivalInterval;
    double priorityClientRate;
    int clientPatienceTime;

public:
    SimulationEntry(int simulationDuration,
                    int cashierCount,
                    int minServiceTime,
                    int maxServiceTime,
                    int clientArrivalInterval,
                    double priorityClientRate,
                    int clientPatienceTime);

    int getSimulationDuration() const;
    int getCashierCount() const;
    int getMinServiceTime() const;
    int getMaxServiceTime() const;
    int getClientArrivalInterval() const;
    double getPriorityClientRate() const;
    int getClientPatienceTime() const;
};

#endif
