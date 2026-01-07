#include "client.hpp"
#include "crypto_utils.hpp"
#include "range_proof_utils.hpp"
#include "iostream"
#include <string>
#include <format>

// Use std::array for modern C++
using ClientID = std::array<unsigned char, 32>;

// Example ID initialization (replace with actual ID)
ClientID client_id = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

std::string toHex(const std::array<unsigned char, 32>& bytes) {
    std::string hex;
    hex.reserve(64); // Pre-reserve 64 chars (32 bytes * 2 chars/byte)
    for (unsigned char b : bytes) {
        // {:02x} ensures 2-digit lowercase hex with leading zeros
        hex += std::format("{:02x}", b);
    }
    return hex;
}

int main() {
    boost::asio::io_context io;
    uint8_t clientPriv[32] = {0x01};
    uint8_t serverPriv[32] = {0x02};
    uint8_t serverPub[64];
    crypto_utils::derive_pubkey(serverPriv, serverPub);

    Client client(io, "CLIENT-001", clientPriv, serverPub, "127.0.0.1", 9000);
    client.authenticate();
    client.generate_proof();
    client.sendSecure(toHex(client_id), "Hello from client!");

    SecureData data = client.receiveSecure();
    std::cout << data.sender << ": " << data.payload << "\n";
}
