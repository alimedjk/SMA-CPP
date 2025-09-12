#pragma once
#include "client/AbstractClient.hpp"
namespace bank::client {
class VIPClient final : public AbstractClient {
public:
    using AbstractClient::AbstractClient;
    bool isPriority() const noexcept override { return true; }
    std::string typeName() const override { return "VIPClient"; }
};
} // namespace bank::client
