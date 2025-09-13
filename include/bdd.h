#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <libpq-fe.h>

// 前置声明，避免头文件循环
class Simulation;
class SimulationEntry;

class Database {
public:
    // 与实现一致：按值接收，避免你之前的签名不匹配
    explicit Database(std::string conninfo);
    ~Database();

    // 只在头文件内联定义一次，避免重定义
    bool isConnected() const { return conn_ && PQstatus(conn_) == CONNECTION_OK; }

    // 写入（你已有）
    void insertSimulationResults(const Simulation& simulation, const SimulationEntry& entry);

    // 新增：读取能力
    // 列出所有 simulation 结果的 id（按降序）
    std::vector<int> listSimulationIds() const;

    // 读取某个 id 的 served / non_served，返回 true 表示取到
    bool getServedVsNonServed(int id, int& served, int& nonServed) const;

private:
    PGconn* conn_{nullptr};
};

#endif
