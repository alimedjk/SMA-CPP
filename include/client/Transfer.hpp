#pragma once
#include "inculde/client/AbstractOperation.hpp"
namespace bank::client {
class Transfer final : public AbstractOperation {
public:
    explicit Transfer(int serviceTime, bool urgent = true);
    std::string name() const override { return "Transfer"; }
    bool isUrgent() const override { return urgent_; }
private:
    bool urgent_{true};
};
} // namespace bank::client
