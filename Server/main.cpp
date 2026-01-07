#include "server.hpp"
#include "crypto_utils.hpp"
#include "range_proof_utils.hpp"
#include "iostream"

int main() {
    boost::asio::io_context io;
    uint8_t serverPriv[32] = {0x02};
    uint8_t clientPub[64];
    uint8_t clientPriv[32] = {0x01};
    crypto_utils::derive_pubkey(clientPriv, clientPub);

    Server server(io, 9000, serverPriv);
    server.registerClient("CLIENT-001", clientPub);
    server.run();
    server.rangeProofClient();

    SecureData data = server.receiveSecure();
    std::cout << data.sender << ": " << data.payload << std::endl;
    server.sendSecure("Server","Hello!");
}
