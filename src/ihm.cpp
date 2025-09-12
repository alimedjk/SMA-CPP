// src/ihm.cpp
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <fstream>

#include <libpq-fe.h>

#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "matplotlibcpp.h"
#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

namespace plt = matplotlibcpp;

// ---------- 小工具 ----------
template<typename F>
inline void safe_call(const char* name, F&& f){
    try { f(); }
    catch(const std::exception& e){
        std::cerr << name << " skipped: " << e.what() << "\n";
    }
}
static bool file_exists(const char* p){
    std::ifstream ifs(p, std::ios::binary);
    return (bool)ifs;
}

// ---------- DB 工具 ----------
void exitOnError(PGconn* conn, PGresult* res, const std::string &msg) {
    if (res) PQclear(res);
    std::cerr << msg << " : " << PQerrorMessage(conn) << std::endl;
    PQfinish(conn);
    std::exit(EXIT_FAILURE);
}

PGconn* connectDB(const std::string &conninfo) {
    PGconn* conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Erreur de connexion: " << PQerrorMessage(conn) << std::endl;
        if (conn) PQfinish(conn);
        return nullptr;
    }
    return conn;
}

// 直接从 resultats_simulation 取最近 N 个 id（而不是 simulation_entry）
std::vector<int> fetchResultIds(PGconn* conn, int limit = 5) {
    std::vector<int> ids;
    std::ostringstream q;
    q << "SELECT id FROM resultats_simulation ORDER BY id DESC LIMIT " << limit << ";";
    PGresult* res = PQexec(conn, q.str().c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        exitOnError(conn, res, "Erreur SELECT ids from resultats_simulation");
    }
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        ids.push_back(std::stoi(PQgetvalue(res, i, 0)));
    }
    PQclear(res);
    std::reverse(ids.begin(), ids.end()); // 升序
    return ids;
}

struct ResultRow { int id; int clients_servis; int clients_non_servis; double taux_satisfaction; };

std::vector<ResultRow> fetchResultsForIds(PGconn* conn, const std::vector<int>& ids) {
    std::vector<ResultRow> out;
    if (ids.empty()) return out;

    std::ostringstream q;
    q << "SELECT id, clients_servis, clients_non_servis, taux_satisfaction "
      << "FROM resultats_simulation WHERE id IN (";
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i) q << ",";
        q << ids[i];
    }
    q << ") ORDER BY id;";
    PGresult* res = PQexec(conn, q.str().c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        exitOnError(conn, res, "Erreur SELECT results");
    }
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        ResultRow r;
        r.id = std::stoi(PQgetvalue(res, i, 0));
        r.clients_servis = std::stoi(PQgetvalue(res, i, 1));
        r.clients_non_servis = std::stoi(PQgetvalue(res, i, 2));
        double ts = std::stod(PQgetvalue(res, i, 3));
        if (ts <= 1.0) ts *= 100.0; // 0..1 -> %
        r.taux_satisfaction = ts;
        out.push_back(r);
    }
    PQclear(res);
    return out;
}

ResultRow fetchResultForId(PGconn* conn, int id) {
    std::ostringstream q;
    q << "SELECT id, clients_servis, clients_non_servis, taux_satisfaction "
      << "FROM resultats_simulation WHERE id = " << id << " LIMIT 1;";
    PGresult* res = PQexec(conn, q.str().c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        exitOnError(conn, res, "Erreur SELECT single result");
    }
    ResultRow r;
    r.id = std::stoi(PQgetvalue(res, 0, 0));
    r.clients_servis = std::stoi(PQgetvalue(res, 0, 1));
    r.clients_non_servis = std::stoi(PQgetvalue(res, 0, 2));
    double ts = std::stod(PQgetvalue(res, 0, 3));
    if (ts <= 1.0) ts *= 100.0;
    r.taux_satisfaction = ts;
    PQclear(res);
    return r;
}

static double vec_max(const std::vector<double>& v){
    double m = 0.0;
    for(double x : v) if (x > m) m = x;
    return m;
}

int main(int, char**) {
    // 强制无界面后端（必须在第一次使用 plt 之前）
    setenv("MPLBACKEND", "Agg", 1);

    std::string conninfo =
        "host=postgresql-hammal.alwaysdata.net port=5432 dbname=hammal_simulation user=hammal password=Zahrdin.99";
    PGconn* conn = connectDB(conninfo);
    if (!conn) return EXIT_FAILURE;

    // 只看 resultats_simulation 里的最近 N 条
    std::vector<int> ids = fetchResultIds(conn, 5);
    if (ids.empty()) {
        std::cerr << "Aucune ligne dans resultats_simulation." << std::endl;
        PQfinish(conn);
        return EXIT_FAILURE;
    }

    auto rows = fetchResultsForIds(conn, ids);

    // ---------- 图1：分组柱状图（不使用 xticks） ----------
    std::vector<double> x;
    std::vector<double> servis, non_servis;
    for (size_t i = 0; i < rows.size(); ++i) {
        x.push_back(static_cast<double>(i));
        servis.push_back(rows[i].clients_servis);
        non_servis.push_back(rows[i].clients_non_servis);
    }
    std::vector<double> x_servis, x_non_servis;
    double width = 0.35;
    for (double xi : x) {
        x_servis.push_back(xi - width/2.0);
        x_non_servis.push_back(xi + width/2.0);
    }

    safe_call("figure(1)", [&]{ plt::figure(); });
    safe_call("bar(servis)",     [&]{ plt::bar(x_servis,    servis,     "", "Servis",      width); });
    safe_call("bar(non_servis)", [&]{ plt::bar(x_non_servis,non_servis, "", "Non servis",  width); });
    safe_call("legend()",        [&]{ plt::legend(); });
    safe_call("title(1)",        [&]{ plt::title("Clients servis / non servis (dernieres simulations)"); });
    safe_call("xlabel(1)",       [&]{ plt::xlabel("Index (0..N-1)"); });
    safe_call("ylabel(1)",       [&]{ plt::ylabel("Nombre de clients"); });

    double ymax = std::max(vec_max(servis), vec_max(non_servis));
    for (size_t i = 0; i < x.size(); ++i){
        safe_call("text(id)", [&]{
            std::ostringstream s; s << "id=" << rows[i].id;
            plt::text(x[i]-0.15, -0.05 * std::max(1.0, ymax), s.str());
        });
        safe_call("text(servis)",     [&]{ plt::text(x_servis[i],     servis[i]     + 0.02*ymax, std::to_string((int)servis[i])); });
        safe_call("text(non_servis)", [&]{ plt::text(x_non_servis[i], non_servis[i] + 0.02*ymax, std::to_string((int)non_servis[i])); });
    }
    safe_call("save(bars.png)",  [&]{ plt::save("bin/bars.png"); });

    int last_id = rows.back().id; // rows according  resultats_simulation 排的
    ResultRow single = fetchResultForId(conn, last_id);
    std::vector<double> sx{0.0, 1.0};
    std::vector<double> sy{ single.taux_satisfaction, 100.0 - single.taux_satisfaction };

    safe_call("figure(2)",       [&]{ plt::figure(); });
    safe_call("bar(satisf)",     [&]{ plt::bar(sx, sy, "", "", 0.6); });
    safe_call("ylim()",          [&]{ plt::ylim(0, 100); });
    safe_call("text(label1)",    [&]{ plt::text(sx[0]-0.1, -5.0, "Satisfaits"); });
    safe_call("text(label2)",    [&]{ plt::text(sx[1]-0.2, -5.0, "Non satisfaits"); });
    safe_call("text(val1)",      [&]{ plt::text(sx[0]-0.05, sy[0] + 2.0, std::to_string((int)sy[0]) + "%"); });
    safe_call("text(val2)",      [&]{ plt::text(sx[1]-0.05, sy[1] + 2.0, std::to_string((int)sy[1]) + "%"); });
    safe_call("title(2)",        [&]{ plt::title("Taux de satisfaction (id=" + std::to_string(last_id) + ")"); });
    safe_call("save(satis.png)", [&]{ plt::save("bin/satisfaction.png"); });

    std::cout << "Tried saving:\n"
              << "  bin/bars.png  -> " << (file_exists("bin/bars.png") ? "OK" : "FAILED") << "\n"
              << "  bin/satisfaction.png -> " << (file_exists("bin/satisfaction.png") ? "OK" : "FAILED") << "\n";

    PQfinish(conn);
    return 0;
}
