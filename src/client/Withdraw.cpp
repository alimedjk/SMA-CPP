#include "client/Withdraw.hpp"
namespace bank::client {
Withdraw::Withdraw(int serviceTime, bool urgent)
    : AbstractOperation(serviceTime), urgent_(urgent) {}
} // namespace bank::client
