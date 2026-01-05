#pragma once

#include <boost/asio.hpp>
#include <map>
#include <string>
#include "auth.pb.h"
#include "data.pb.h"

using ByteVec = std::vector<uint8_t>;
using boost::asio::ip::tcp;

class Client {
    uint8_t priv[32];
    uint8_t serverPub[64];
    std::string serial;
    tcp::socket socket_;

public:
    Client(boost::asio::io_context& io,
           const std::string& serialId,
           const uint8_t privKey[32],
           const uint8_t serverPubKey[64],
           const std::string& host,
           uint16_t port);

    void authenticate();
    void sendSecure(const std::string& sender, const std::string& payload);
    std::string receiveSecure();

private:
    ClientHello createHello();
    ClientResponse respondToChallenge(const ServerChallenge& ch);
};

