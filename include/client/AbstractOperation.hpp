#pragma once
#include <string>
#include <memory>
#include "client/AbstractOperation.hpp"
namespace bank::client {
class AbstractOperation {
public:
    explicit AbstractOperation(int serviceTime);
    virtual ~AbstractOperation() = default;
    virtual std::string name() const = 0;
    virtual bool isUrgent() const = 0;
    int serviceTime() const noexcept { return serviceTime_; }
    void setServiceTime(int t) noexcept { serviceTime_ = t; }
    AbstractOperation(const AbstractOperation&) = delete;
    AbstractOperation& operator=(const AbstractOperation&) = delete;
    AbstractOperation(AbstractOperation&&) noexcept = default;
    AbstractOperation& operator=(AbstractOperation&&) noexcept = default;
protected:
    int serviceTime_{0};
};
using OperationPtr = std::unique_ptr<AbstractOperation>;
} // namespace bank::client
