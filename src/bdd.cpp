#include "bdd.h"
#include <iostream>
#include <sstream>
#include "simulation/Simulation.h"
#include "simulation/SimulationEntry.h"

Database::Database(const std::string& connInfo) {
    conn_ = PQconnectdb(connInfo.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::cerr << "Connexion échouée : " << PQerrorMessage(conn_) << std::endl;
        conn_ = nullptr;
    }
}

Database::~Database() {
    if (conn_) PQfinish(conn_);
}

bool Database::isConnected() const {
    return conn_ != nullptr;
}

// ?? NOUVELLE MÉTHODE
void Database::insertSimulationResults(const Simulation& simulation, const SimulationEntry& entry) {
    const auto& stats = simulation.getStatisticManager();

    std::ostringstream query;
    query << "INSERT INTO resultats_simulation("
        << "duree_effective, clients_servis, clients_non_servis, taux_satisfaction, "
        << "temps_attente_moyen, temps_service_moyen, taux_occupation)"
        << " VALUES ("
        << entry.getSimulationDuration() << ", "
        << stats.servedClientCount() << ", "
        << stats.nonServedClientCount() << ", "
        << stats.calculateClientSatisfactionRate() << ", "
        << stats.calculateAverageClientWaitingTime() << ", "
        << stats.calculateAverageClientServiceTime() << ", "
        << stats.calculateAverageCashierOccupationRate(entry.getCashierCount())
        << ");";

    PGresult* res = PQexec(conn_, query.str().c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Erreur d'insertion : " << PQerrorMessage(conn_) << std::endl;
    }
    else {
        std::cout << "Résultats insérés avec succès ?" << std::endl;
    }
    PQclear(res);
}
