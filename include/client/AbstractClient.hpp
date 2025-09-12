#pragma once
#include <memory>
#include <string>
#include <algorithm>

#include "client/AbstractOperation.hpp"
namespace bank::client {
class AbstractClient {
public:
    AbstractClient(int arrivalTime, int patienceTime, OperationPtr operation = nullptr);
    virtual ~AbstractClient() = default;
    void reducePatience(int delta = 1) noexcept;
    bool isPatient() const noexcept;
    virtual bool isPriority() const noexcept = 0;
    int arrivalTime() const noexcept { return arrivalTime_; }
    void setArrivalTime(int t) noexcept { arrivalTime_ = t; }
    int serviceStartTime() const noexcept { return serviceStartTime_; }
    void setServiceStartTime(int t) noexcept { serviceStartTime_ = t; }
    int departureTime() const noexcept { return departureTime_; }
    void setDepartureTime(int t) noexcept { departureTime_ = t; }
    int patienceTime() const noexcept { return patienceTime_; }
    void setPatienceTime(int t) noexcept { patienceTime_ = t; }
    const AbstractOperation* operation() const noexcept { return operation_.get(); }
    AbstractOperation* operation() noexcept { return operation_.get(); }
    bool hasOperation() const noexcept { return static_cast<bool>(operation_); }
    void setOperation(OperationPtr op) noexcept { operation_ = std::move(op); }
    OperationPtr releaseOperation() noexcept { return std::move(operation_); }
    virtual std::string typeName() const = 0;
    AbstractClient(const AbstractClient&) = delete;
    AbstractClient& operator=(const AbstractClient&) = delete;
    AbstractClient(AbstractClient&&) noexcept = default;
    AbstractClient& operator=(AbstractClient&&) noexcept = default;
protected:
    int arrivalTime_{0};
    int serviceStartTime_{-1};
    int departureTime_{-1};
    int patienceTime_{0};
    OperationPtr operation_{nullptr};
};
} // namespace bank::client
