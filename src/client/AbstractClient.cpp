#include "client/AbstractClient.hpp"
namespace bank::client {
AbstractClient::AbstractClient(int arrivalTime, int patienceTime, OperationPtr operation)
    : arrivalTime_(arrivalTime),
      patienceTime_(std::max(0, patienceTime)),
      operation_(std::move(operation)) {}
void AbstractClient::reducePatience(int delta) noexcept {
    if (delta <= 0) return;
    patienceTime_ = std::max(0, patienceTime_ - delta);
}
bool AbstractClient::isPatient() const noexcept {
    return patienceTime_ > 0;
}
} // namespace bank::client
