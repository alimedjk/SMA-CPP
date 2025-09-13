#include "bdd.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "simulation/Simulation.h"
#include "simulation/SimulationEntry.h"

Database::Database(std::string conninfo) {
    conn_ = PQconnectdb(conninfo.c_str());
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
        std::cerr << "DB connect failed.\nUsing conn: " << conninfo << "\n";
        std::cerr << (conn_ ? PQerrorMessage(conn_) : "no connection object") << std::endl;
        if (conn_) { PQfinish(conn_); conn_ = nullptr; }
    }
}

Database::~Database() {
    if (conn_) PQfinish(conn_);
}

void Database::insertSimulationResults(const Simulation& simulation, const SimulationEntry& entry) {
    if (!isConnected()) {
        std::cerr << "DB not connected, skip insert.\n";
        return;
    }

    const auto& stats = simulation.getStatisticManager();

    // 插入 resultats_simulation 并返回生成的 id
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
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Erreur d'insertion resultats : " << (res ? PQerrorMessage(conn_) : "no result") << std::endl;
        if (res) PQclear(res);
        return;
    }

    int simulation_resultat_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // 用上面的 id 插入 simulation_entry
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
    if (!res_entry || PQresultStatus(res_entry) != PGRES_COMMAND_OK) {
        std::cerr << "Erreur d'insertion entry : " << (res_entry ? PQerrorMessage(conn_) : "no result") << std::endl;
    } else {
        std::cout << "entry insérées avec succès." << std::endl;
    }
    if (res_entry) PQclear(res_entry);
}

std::vector<int> Database::listSimulationIds() const {
    std::vector<int> ids;
    if (!isConnected()) return ids;

    const char* sql = "SELECT id FROM resultats_simulation ORDER BY id DESC;";
    PGresult* res = PQexec(conn_, sql);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Erreur SELECT id : " << (res ? PQerrorMessage(conn_) : "no result") << std::endl;
        if (res) PQclear(res);
        return ids;
    }

    int rows = PQntuples(res);
    ids.reserve(rows);
    for (int i = 0; i < rows; ++i) {
        ids.push_back(std::stoi(PQgetvalue(res, i, 0)));
    }
    PQclear(res);
    return ids;
}

bool Database::getServedVsNonServed(int id, int& served, int& nonServed) const {
    served = 0; nonServed = 0;
    if (!isConnected()) return false;

    std::ostringstream oss;
    oss << "SELECT clients_servis, clients_non_servis "
        << "FROM resultats_simulation WHERE id=" << id << ";";
    PGresult* res = PQexec(conn_, oss.str().c_str());
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) < 1) {
        std::cerr << "Erreur SELECT served/non_served for id=" << id << " : "
                  << (res ? PQerrorMessage(conn_) : "no result") << std::endl;
        if (res) PQclear(res);
        return false;
    }
    served     = std::stoi(PQgetvalue(res, 0, 0));
    nonServed  = std::stoi(PQgetvalue(res, 0, 1));
    PQclear(res);
    return true;
}
