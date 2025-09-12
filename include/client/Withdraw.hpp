#pragma once
#include "client/AbstractOperation.hpp"
namespace bank::client {
class Withdraw final : public AbstractOperation {
public:
    explicit Withdraw(int serviceTime, bool urgent = true);
    std::string name() const override { return "Withdraw"; }
    bool isUrgent() const override { return urgent_; }
private:
    bool urgent_{true};
};
} // namespace bank::client
