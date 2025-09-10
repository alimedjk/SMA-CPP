#include "include/client/Transfer.hpp"
namespace bank::client {
Transfer::Transfer(int serviceTime, bool urgent)
    : AbstractOperation(serviceTime), urgent_(urgent) {}
} // namespace bank::client
