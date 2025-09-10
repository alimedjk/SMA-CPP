#include "Simulation.h"
#include "SimulationUtility.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Simulation::Simulation(const SimulationEntry& entry)
    : simulationEntry(entry), currentTime(0) {
    cashiers.resize(entry.getCashierCount(), nullptr);
}

void Simulation::simulate() {
    for (currentTime = 0; currentTime < simulationEntry.getSimulationDuration(); currentTime++) {
        // Enregistre le temps
        statisticManager.simulationDurationRecord();

        // Génération de nouveaux clients
        if (currentTime % simulationEntry.getClientArrivalInterval() == 0) {
            generateClient();
        }

        // Mise à jour des files d'attente
        updateWaitingQueue();

        // Traitement des caissiers
        processCashiers();
    }
}

void Simulation::generateClient() {
    bool isPriority = SimulationUtility::probabilityTest(simulationEntry.getPriorityClientRate());

    // Création d’un client (⚠️ tu devras créer une classe Client dérivant de AbstractClient)
    AbstractClient* client = new AbstractClient(currentTime, isPriority, simulationEntry.getClientPatienceTime());

    if (isPriority) {
        waitingQueue.insert(waitingQueue.begin(), client);
    } else {
        waitingQueue.push_back(client);
    }
}

void Simulation::updateWaitingQueue() {
    // On enlève les clients qui dépassent leur patience
    auto it = waitingQueue.begin();
    while (it != waitingQueue.end()) {
        AbstractClient* client = *it;
        if (currentTime - client->getArrivalTime() > simulationEntry.getClientPatienceTime()) {
            statisticManager.registerNonServedClient(client);
            it = waitingQueue.erase(it);
        } else {
            ++it;
        }
    }
}

void Simulation::processCashiers() {
    for (size_t i = 0; i < cashiers.size(); i++) {
        AbstractClient* client = cashiers[i];

        if (client == nullptr) {
            // Si le caissier est libre et qu’il y a un client
            if (!waitingQueue.empty()) {
                AbstractClient* nextClient = waitingQueue.front();
                waitingQueue.erase(waitingQueue.begin());

                nextClient->setServiceStartTime(currentTime);
                nextClient->setDepartureTime(
                    currentTime + SimulationUtility::generateRandomInt(
                        simulationEntry.getMinServiceTime(),
                        simulationEntry.getMaxServiceTime()
                    )
                );

                cashiers[i] = nextClient;
                statisticManager.registerServedClient(nextClient);
            }
        } else {
            // Si le client a terminé son service
            if (client->getDepartureTime() <= currentTime) {
                cashiers[i] = nullptr;
            } else {
                statisticManager.cashierOccupationRecord();
            }
        }
    }
}

std::string Simulation::simulationResults() const {
    std::ostringstream oss;
    oss << "Résultats de la simulation:\n";
    oss << "Durée effective: " << simulationEntry.getSimulationDuration() << "\n";
    oss << "Clients servis: " << statisticManager.servedClientCount() << "\n";
    oss << "Clients non servis: " << statisticManager.nonServedClientCount() << "\n";
    oss << "Taux satisfaction: " << statisticManager.calculateClientSatisfactionRate() << "%\n";
    oss << "Temps d’attente moyen: " << statisticManager.calculateAverageClientWaitingTime() << "\n";
    oss << "Temps de service moyen: " << statisticManager.calculateAverageClientServiceTime() << "\n";
    oss << "Taux d’occupation caissiers: "
        << statisticManager.calculateAverageCashierOccupationRate(simulationEntry.getCashierCount()) << "%\n";
    return oss.str();
}
