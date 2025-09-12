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

void Database::insertSimulationResults(const Simulation& simulation, const SimulationEntry& entry) {
    const auto& stats = simulation.getStatisticManager();

    // Insertion dans resultats_simulation et récupération de l'id généré
    std::ostringstream query;
    query << "INSERT INTO resultats_simulation ("
        << "duree_effective, clients_servis, clients_non_servis, taux_satisfaction, "
        << "temps_attente_moyen, temps_service_moyen, taux_occupation) "
        << "VALUES ("
        << entry.getSimulationDuration() << ", "
        << stats.servedClientCount() << ", "
        << stats.nonServedClientCount() << ", "
        << stats.calculateClientSatisfactionRate() << ", "
        << stats.calculateAverageClientWaitingTime() << ", "
        << stats.calculateAverageClientServiceTime() << ", "
        << stats.calculateAverageCashierOccupationRate(entry.getCashierCount())
        << ") RETURNING id;";

    PGresult* res = PQexec(conn_, query.str().c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Erreur d'insertion resultats : " << PQerrorMessage(conn_) << std::endl;
        PQclear(res);
        return;
    }

    // Récupération de l'id généré
    int simulation_resultat_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Insertion dans simulation_entry avec l'id correct
    std::ostringstream query_entry;
    query_entry << "INSERT INTO simulation_entry ("
        << "id, duree_effective, cashiercount, minservicetime, maxservicetime, "
        << "clientarrivalinterval, priorityclientrate, clientpatiencetime) "
        << "VALUES ("
        << simulation_resultat_id << ", "
        << entry.getSimulationDuration() << ", "
        << entry.getCashierCount() << ", "
        << entry.getMinServiceTime() << ", "
        << entry.getMaxServiceTime() << ", "
        << entry.getClientArrivalInterval() << ", "
        << entry.getPriorityClientRate() << ", "
        << entry.getClientPatienceTime()
        << ");";

    PGresult* res_entry = PQexec(conn_, query_entry.str().c_str());
    if (PQresultStatus(res_entry) != PGRES_COMMAND_OK) {
        std::cerr << "Erreur d'insertion entry : " << PQerrorMessage(conn_) << std::endl;
    }
    else {
        std::cout << "entry inseres avec succes ?" << std::endl;
    }
    PQclear(res_entry);

}