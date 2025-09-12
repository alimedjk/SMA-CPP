// src/ihm.cpp  —— 纯 C++ 画三张图：分组柱状、饼图、单条柱状
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
static bool truthy_env(const char* k){
    if (const char* v = std::getenv(k)) {
        std::string s(v);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return (s=="1" || s=="true" || s=="yes");
    }
    return false;
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

// 直接从 resultats_simulation 取最近 N 个 id
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
    // 交互 or 离线(保存文件) 模式
    const bool interactive = truthy_env("IHM_SHOW");
    if (!interactive) {
        // 强制无界面后端（必须在第一次使用 plt 之前）
        setenv("MPLBACKEND", "Agg", 1);
        matplotlibcpp::backend("Agg");
    }

    std::string conninfo =
        "host=postgresql-hammal.alwaysdata.net port=5432 dbname=hammal_simulation user=hammal password=Zahrdin.99";
    PGconn* conn = connectDB(conninfo);
    if (!conn) return EXIT_FAILURE;

    std::vector<int> ids = fetchResultIds(conn, 5);
    if (ids.empty()) {
        std::cerr << "Aucune ligne dans resultats_simulation." << std::endl;
        PQfinish(conn);
        return EXIT_FAILURE;
    }
    auto rows = fetchResultsForIds(conn, ids);

    // ---------- 图1：分组柱状图 ----------
    std::vector<double> x;
    std::vector<double> servis, non_servis;
    for (size_t i = 0; i < rows.size(); ++i) {
        x.push_back(static_cast<double>(i));
        servis.push_back(rows[i].clients_servis);
        non_servis.push_back(rows[i].clients_non_servis);
    }
    // 用左右平移来“做宽度”，关键字里只放 label（matplotlibcpp 的 bar 本身没有 width 参数）
    const double half = 0.18;
    std::vector<double> x_left, x_right;
    for (double xi : x) { x_left.push_back(xi - half); x_right.push_back(xi + half); }

    safe_call("figure(1)", [&]{ plt::figure(); });
    safe_call("bar(servis)", [&]{ plt::bar(x_left, servis, "black", "-", 1.0, {{"label","Servis"}}); });
    safe_call("bar(non_servis)", [&]{ plt::bar(x_right, non_servis, "black", "-", 1.0, {{"label","Non servis"}}); });

    // x 轴显示 id
    std::vector<std::string> id_labels; id_labels.reserve(rows.size());
    for (auto &r : rows) id_labels.push_back(std::to_string(r.id));
    safe_call("xticks", [&]{ plt::xticks(x, id_labels); });

    safe_call("legend()", [&]{ plt::legend(); });
    safe_call("title(1)", [&]{ plt::title("Clients servis / non servis par simulation"); });
    safe_call("xlabel(1)", [&]{ plt::xlabel("ID simulation"); });
    safe_call("ylabel(1)", [&]{ plt::ylabel("Nombre de clients"); });
    safe_call("tight", [&]{ plt::tight_layout(); });
    safe_call("save(bars)", [&]{ plt::save("bin/clients_bars.png", 150); });

    // ---------- 图2：满意度饼图（最近一次） ----------
    int last_id = rows.back().id;
    ResultRow last = fetchResultForId(conn, last_id);
    const double sat = last.taux_satisfaction;
    const double uns = 100.0 - sat;

    safe_call("figure(2)", [&]{ plt::figure(); });
    safe_call("pie", [&]{ plt::pie(std::vector<double>{sat, uns},
                                   std::vector<std::string>{"Satisfaits","Non satisfaits"},
                                   90.0, "%.1f%%"); });
    safe_call("title(2)", [&]{ plt::title("Taux de satisfaction (id=" + std::to_string(last_id) + ") - " + std::to_string((int)std::round(sat)) + "%"); });
    safe_call("tight", [&]{ plt::tight_layout(); });
    safe_call("save(pie)", [&]{ plt::save("bin/satisfaction_pie.png", 150); });

    // ---------- 图3：最近一次的双柱图 ----------
    safe_call("figure(3)", [&]{ plt::figure(); });
    std::vector<double> sx{0.0, 1.0};
    std::vector<double> sy{static_cast<double>(last.clients_servis),
                           static_cast<double>(last.clients_non_servis)};
    safe_call("bar(single)", [&]{ plt::bar(sx, sy, "black", "-", 1.0, {{"label","Clients"}}); });
    safe_call("xticks(single)", [&]{ plt::xticks(sx, std::vector<std::string>{"Servis","Non servis"}); });
    safe_call("title(3)", [&]{ plt::title("Clients (id=" + std::to_string(last_id) + ")"); });
    safe_call("ylabel(3)", [&]{ plt::ylabel("Nombre de clients"); });
    safe_call("tight", [&]{ plt::tight_layout(); });
    safe_call("save(single)", [&]{ plt::save("bin/clients_bars_single.png", 150); });

    if (interactive) {
        // 在需要演示时弹窗（WSLg 或装了 X server 才能看到）
        safe_call("show", [&]{ plt::show(); });
    }

    std::cout << "Saved:\n"
              << "  bin/clients_bars.png           -> " << (file_exists("bin/clients_bars.png") ? "OK" : "FAILED") << "\n"
              << "  bin/satisfaction_pie.png       -> " << (file_exists("bin/satisfaction_pie.png") ? "OK" : "FAILED") << "\n"
              << "  bin/clients_bars_single.png    -> " << (file_exists("bin/clients_bars_single.png") ? "OK" : "FAILED") << "\n";

    PQfinish(conn);
    return 0;
}
