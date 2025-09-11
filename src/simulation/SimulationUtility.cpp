#include "simulation/SimulationUtility.h"
#include <random>

int SimulationUtility::generateRandomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

bool SimulationUtility::probabilityTest(double probability) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(0.0, 1.0);
    return distrib(gen) < probability;
}
