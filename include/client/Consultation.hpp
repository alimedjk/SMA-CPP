#pragma once
#include "include/client/AbstractOperation.hpp"
namespace bank::client {
class Consultation final : public AbstractOperation {
public:
    explicit Consultation(int serviceTime);
    std::string name() const override { return "Consultation"; }
    bool isUrgent() const override { return false; }
};
} // namespace bank::client
