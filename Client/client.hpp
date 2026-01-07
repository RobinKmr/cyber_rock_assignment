#pragma once

#include <boost/asio.hpp>
#include <map>
#include <string>
#include "auth.pb.h"
#include "data.pb.h"

using boost::asio::ip::tcp;

#define HIDDEN_NO   12345678 // Hidden number
#define NOF_ROOTS   4 // Max number of roots for value x
#define N           32 // Max numbers of bits for X
#define A           1 // Any number between 1 to (2^32-1)
#define B           (1ULL << N) - 1 // Any number between 1 to (2^32-1)

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
    void generate_proof();
    void sendSecure(const std::string& sender, const std::string& payload);
    SecureData receiveSecure();

private:
    ClientHello createHello();
    ClientResponse respondToChallenge(const ServerChallenge& ch);
};

