#ifndef SIMULATION_UTILITY_H
#define SIMULATION_UTILITY_H

#include <random>

class SimulationUtility {
public:
    static int generateRandomInt(int min, int max);
    static bool probabilityTest(double probability);
};

#endif
