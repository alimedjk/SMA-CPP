// src/ihm.cpp  —— 读取 PostgreSQL 结果，生成条形图（对比若干仿真）+ 单个仿真的饼图
// 支持 --ids=1,2,3 选择对比的仿真ID，--id=2 为饼图的目标仿真ID，--show 选择是否弹窗显示
// WSL 下如需弹窗显示：Windows 端运行 VcXsrv；WSL 安装 python3-tk；并在程序首次绘图前设置 TkAgg 后端。

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <filesystem>
#include <libpq-fe.h>

#include "third_party/matplotlibcpp.h"
namespace plt = matplotlibcpp;

static const char* kConnInfo =
    "host=postgresql-hammal.alwaysdata.net "
    "port=5432 "
    "dbname=hammal_simulation "
    "user=hammal "
    "password=Zahrdin.99";

static inline std::string trim(std::string s) {
    auto notspace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());
    return s;
}

static inline std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, sep)) out.push_back(trim(item));
    return out;
}

struct SimSummary {
    int id = -1;
    int served = 0;
    int unserved = 0;
    double satisfaction = 0.0; // 数据库里是 numeric，按百分比（如 92.0）
};

//-----------------------------
static inline void clearAndWarn(PGresult* r, const std::string& what) {
    if (r) PQclear(r);
    std::cerr << what << "\n";
}

static bool fetch_summary(PGconn* conn, int simId, SimSummary& out) {
    const char* sql =
        "SELECT clients_servis, clients_non_servis, taux_satisfaction "
        "FROM resultats_simulation WHERE id = $1";

    std::string idstr = std::to_string(simId);
    const char* params[1] = { idstr.c_str() };

    PGresult* res = PQexecParams(conn, sql,
                                 1,      // nParams
                                 nullptr,// param types
                                 params, // param values
                                 nullptr,// param lengths
                                 nullptr,// param formats
                                 0);     // result format: text

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string emsg = "[SQL] fetch_summary id=" + std::to_string(simId) +
                           " failed: " + PQerrorMessage(conn);
        clearAndWarn(res, emsg);
        return false;
    }

    if (PQntuples(res) < 1) {
        clearAndWarn(res, "[warn] simulation id=" + std::to_string(simId) + " not found.");
        return false;
    }

    out.id = simId;
    out.served    = std::atoi(PQgetvalue(res, 0, 0));
    out.unserved  = std::atoi(PQgetvalue(res, 0, 1));
    out.satisfaction = std::atof(PQgetvalue(res, 0, 2)); // 92, 93.0 等

    PQclear(res);
    return true;
}

static void plot_compare_bar(const std::vector<SimSummary>& sims,
                             bool can_show,
                             const std::string& outdir) {
    if (sims.empty()) return;

    // x 位置：0..n-1
    std::vector<double> x;
    x.reserve(sims.size());
    for (size_t i = 0; i < sims.size(); ++i) x.push_back(static_cast<double>(i));

    std::vector<double> y_served, y_unserved;
    y_served.reserve(sims.size());
    y_unserved.reserve(sims.size());

    std::vector<std::string> xticklabels;
    xticklabels.reserve(sims.size());

    for (auto& s : sims) {
        y_served.push_back(s.served);
        y_unserved.push_back(s.unserved);
        xticklabels.push_back(std::to_string(s.id));
    }

    plt::figure_size(900, 600);
    const double w = 0.35;

    // 左右两组条
    std::vector<double> x_left = x, x_right = x;
    for (size_t i = 0; i < x.size(); ++i) {
        x_left[i]  = x[i] - w/2.0;
        x_right[i] = x[i] + w/2.0;
    }

    plt::bar(x_left,  y_served, "black", "-", 1.0, {{"label","Servis"}});
    plt::bar(x_right, y_unserved, "black", "-", 1.0, {{"label","Non servis"}});

    plt::title("Clients servis / non servis par simulation");
    plt::xlabel("ID simulation");
    plt::ylabel("Nombre de clients");
    plt::xticks(x, xticklabels);
    plt::legend();
    plt::tight_layout();

    if (can_show) {
        plt::show();
    } else {
        std::string path = outdir + "/compare.png";
        plt::save(path, 200);
        std::cout << "[save] " << path << "\n";
    }
}

static void plot_single_pie(const SimSummary& s,
                            bool can_show,
                            const std::string& outdir,
                            const std::string& filename) {
    // sizes & labels
    std::vector<double> sizes = { static_cast<double>(s.served),
                                  static_cast<double>(s.unserved) };
    std::vector<std::string> labels = {"Servis", "Non servis"};

    plt::figure_size(650, 480);
    plt::pie(sizes, labels, 90.0 /* startangle */, "%.1f%%");


    std::ostringstream oss;
    oss << "Taux de satisfaction (id=" << s.id << ") - "
        << std::fixed << std::setprecision(1) << s.satisfaction << "%";
    plt::title(oss.str());
    plt::tight_layout();

    if (can_show) {
        plt::show();
    } else {
        std::string path = outdir + "/" + filename;
        plt::save(path, 200);
        std::cout << "[save] " << path << "\n";
    }
}

struct Args {
    std::vector<int> ids_to_compare;
    int pie_id = -1;
    bool show = false;
    std::string outdir = "out";
};

static Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string t = argv[i];
        if (t.rfind("--ids=", 0) == 0) {
            auto lst = split(t.substr(6), ',');
            for (auto& s : lst) if (!s.empty()) a.ids_to_compare.push_back(std::stoi(s));
        } else if (t.rfind("--id=", 0) == 0) {
            a.pie_id = std::stoi(t.substr(5));
        } else if (t == "--show") {
            a.show = true;
        } else if (t.rfind("--outdir=", 0) == 0) {
            a.outdir = t.substr(9);
        }
    }
    return a;
}

int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);

    if (args.show) {
        plt::backend("TkAgg");
    } else {
        plt::backend("Agg");
    }

    std::cout << "Using DB conn: " << kConnInfo << "\n";
    PGconn* conn = PQconnectdb(kConnInfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "DB connect failed.\n" << PQerrorMessage(conn);
        PQfinish(conn);
        return 1;
    }
    std::cout << "DB connected.\n";

    namespace fs = std::filesystem;
    std::error_code ec;
    fs::create_directories(args.outdir, ec);   // 等价于 `mkdir -p`
    if (ec) {
        std::cerr << "[warn] cannot create output dir " << args.outdir
                << ": " << ec.message() << "\n";
    }


    std::vector<SimSummary> compare;
    for (int id : args.ids_to_compare) {
        SimSummary s;
        if (fetch_summary(conn, id, s)) compare.push_back(s);
        else std::cerr << "[warn] simulation id=" << id << " not found or SQL failed, skip.\n";
    }
    if (!compare.empty()) {
        plot_compare_bar(compare, args.show, args.outdir);
    }

    // 读取并绘制 单个饼图
    if (args.pie_id >= 0) {
        SimSummary s;
        if (fetch_summary(conn, args.pie_id, s)) {
            plot_single_pie(s, args.show, args.outdir,
                            "pie_" + std::to_string(args.pie_id) + ".png");
        } else {
            std::cerr << "[warn] simulation id=" << args.pie_id
                      << " not found or SQL failed, skip pie.\n";
        }
    }

    PQfinish(conn);
    return 0;
}
