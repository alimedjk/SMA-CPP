#pragma once
#include "include/client/AbstractClient.hpp"
namespace bank::client {
class Client final : public AbstractClient {
public:
    using AbstractClient::AbstractClient;
    bool isPriority() const noexcept override { return false; }
    std::string typeName() const override { return "Client"; }
};
} // namespace bank::client
