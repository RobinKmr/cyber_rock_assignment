#pragma once

#include <boost/asio.hpp>
#include <map>
// #include <functional>
#include <iostream>
#include <auth.pb.h>

using ByteVec = std::vector<uint8_t>;
using boost::asio::ip::tcp;

class Server {
    uint8_t priv[32];
    std::map<std::string, ByteVec> clientKeys;
    std::map<std::string, ByteVec> challenges;
    tcp::acceptor acceptor_;
    tcp::socket socket_;

public:
    Server(boost::asio::io_context& io, uint16_t port, const uint8_t privKey[32]);

    void registerClient(const std::string& serial, const uint8_t pub[64]);
    void run();
    void sendSecure(const std::string& msg);
    std::string receiveSecure();

private:
    ClientHello receiveHello();
    ServerChallenge sendChallenge(const ClientHello& hello);
    ClientResponse receiveResponse(const std::string& serial);
    void authenticateClient();
};
