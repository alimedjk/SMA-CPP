#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <libpq-fe.h>
#include "simulation/Simulation.h"
#include "simulation/SimulationEntry.h"

class Simulation;
class SimulationEntry;

class Database {

public:
    Database(const std::string& connInfo);
    ~Database();
    bool isConnected() const;
    void insertSimulationResults(const Simulation& simulation, const SimulationEntry& entry);
private:
    PGconn* conn_;
};

#endif
