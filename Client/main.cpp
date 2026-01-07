#include "client.hpp"
#include "crypto_utils.hpp"
#include "range_proof_utils.hpp"
#include "iostream"
#include <string>
#include <format>
#include <thread>

using ClientID = std::string;
ClientID client_id = {"12345678901234567890123456789012"};

int main() {
    boost::asio::io_context io;
    uint8_t clientPriv[32] = {0x01};
    uint8_t serverPriv[32] = {0x02};
    uint8_t serverPub[64];
    crypto_utils::derive_pubkey(serverPriv, serverPub);

    Client client(io, client_id, clientPriv, serverPub, "127.0.0.1", 9000);

    client.authenticate();
    client.generate_proof();
    client.sendSecure(client_id, "Hello!");

    SecureData data = client.receiveSecure();
    std::cout << data.sender << ": " << data.payload << "\n";
}
