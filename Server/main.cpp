#include "server.hpp"
#include "crypto_utils.hpp"
#include "range_proof_utils.hpp"
#include "iostream"
#include <thread>

using ClientID = std::string;
ClientID client_id = {"12345678901234567890123456789012"};
uint8_t serverPriv[32] = {0x02};
uint8_t clientPub[64];
uint8_t clientPriv[32] = {0x01};

void handle_client(tcp::socket socket)
{
    try {
        // Derive known client public key (pre-provisioned)
        uint8_t clientPub[64];
        crypto_utils::derive_pubkey(clientPriv, clientPub);

        Server server(std::move(socket), serverPriv);
        server.registerClient(client_id, clientPub);

        // Inject socket into server (reuse accepted socket)
        server.run();
        server.rangeProofClient();

        // -------- Post-auth bidirectional data --------
        SecureData data = server.receiveSecure();
        std::cout << data.sender << ": " << data.payload << std::endl;
        server.sendSecure("Server","Hello!");
    }
    catch (const std::exception& e) {
        std::cerr << "[SERVER] Client error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 9000));

        std::cout << "Server listening on port 9000...\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            // Handle each client in its own thread
            std::thread(
                handle_client,
                std::move(socket)
            ).detach();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[SERVER] Fatal error: " << e.what() << std::endl;
    }
}
