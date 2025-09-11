#include "simulation/Simulation.h"
#include "simulation/SimulationUtility.h"
#include "client/Client.hpp"
#include "client/VIPClient.hpp"
#include "client/Consultation.hpp"
#include "client/Transfer.hpp"
#include "client/Withdraw.hpp"
#include <sstream>
#include <memory>

using bank::client::AbstractClient;
using bank::client::Client;
using bank::client::VIPClient;
using bank::client::AbstractOperation;
using bank::client::OperationPtr;
using bank::client::Consultation;
using bank::client::Transfer;
using bank::client::Withdraw;

static OperationPtr makeRandomOp(int minT, int maxT){
    int st = SimulationUtility::generateRandomInt(minT, maxT);
    switch (SimulationUtility::generateRandomInt(1,3)){
        case 1: return std::make_unique<Consultation>(st);
        case 2: return std::make_unique<Transfer>(st);
        default: return std::make_unique<Withdraw>(st);
    }
}

Simulation::Simulation(const SimulationEntry& entry)
: simulationEntry(entry), currentTime(0){
    cashiers.resize(entry.getCashierCount(), nullptr);
}

void Simulation::simulate(){
    for (currentTime = 0; currentTime < simulationEntry.getSimulationDuration(); ++currentTime){
        statisticManager.simulationDurationRecord();

        if (currentTime % simulationEntry.getClientArrivalInterval() == 0){
            const bool isPri = SimulationUtility::probabilityTest(simulationEntry.getPriorityClientRate());
            auto op = makeRandomOp(simulationEntry.getMinServiceTime(), simulationEntry.getMaxServiceTime());
            AbstractClient* c = isPri
                ? static_cast<AbstractClient*>(new VIPClient(currentTime, simulationEntry.getClientPatienceTime(), std::move(op)))
                : static_cast<AbstractClient*>(new Client  (currentTime, simulationEntry.getClientPatienceTime(), std::move(op)));
            if (isPri) waitingQueue.insert(waitingQueue.begin(), c);
            else       waitingQueue.push_back(c);
        }

        auto it = waitingQueue.begin();
        while (it != waitingQueue.end()){
            AbstractClient* c = *it;
            if (currentTime - c->arrivalTime() > simulationEntry.getClientPatienceTime()){
                statisticManager.registerNonServedClient(c);
                it = waitingQueue.erase(it);
            } else ++it;
        }

        for (size_t i = 0; i < cashiers.size(); ++i){
            AbstractClient* c = cashiers[i];
            if (c == nullptr){
                if (!waitingQueue.empty()){
                    AbstractClient* next = waitingQueue.front();
                    waitingQueue.erase(waitingQueue.begin());
                    next->setServiceStartTime(currentTime);
                    int st = next->operation() ? next->operation()->serviceTime()
                                               : SimulationUtility::generateRandomInt(
                                                     simulationEntry.getMinServiceTime(),
                                                     simulationEntry.getMaxServiceTime());
                    next->setDepartureTime(currentTime + st);
                    cashiers[i] = next;
                    statisticManager.registerServedClient(next);
                }
            } else {
                if (c->departureTime() <= currentTime) cashiers[i] = nullptr;
                else statisticManager.cashierOccupationRecord();
            }
        }
    }
}

std::string Simulation::simulationResults() const{
    std::ostringstream oss;
    oss << "Resultats de la simulation:\n";
    oss << "Duree effective: " << simulationEntry.getSimulationDuration() << "\n";
    oss << "Clients servis: " << statisticManager.servedClientCount() << "\n";
    oss << "Clients non servis: " << statisticManager.nonServedClientCount() << "\n";
    oss << "Taux satisfaction: " << statisticManager.calculateClientSatisfactionRate() << "%\n";
    oss << "Temps d'attente moyen: " << statisticManager.calculateAverageClientWaitingTime() << "\n";
    oss << "Temps de service moyen: " << statisticManager.calculateAverageClientServiceTime() << "\n";
    oss << "Taux d'occupation caissiers: "
        << statisticManager.calculateAverageCashierOccupationRate(simulationEntry.getCashierCount()) << "%\n";
    return oss.str();
}
