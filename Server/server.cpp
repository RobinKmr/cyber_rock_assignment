#include "server.hpp"
#include <crypto_utils.hpp>
#include <nanopb_utils.hpp>
#include <transport.hpp>
#include "auth.pb.h"
#include "data.pb.h"

Server::Server(boost::asio::io_context& io, uint16_t port, const uint8_t privKey[32])
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)), socket_(io) {
    memcpy(priv, privKey, 32);
}

void Server::registerClient(const std::string& serial, const uint8_t pub[64]) {
    clientKeys[serial] = ByteVec(pub, pub + 64);
}

void Server::run() {
    acceptor_.accept(socket_);
    authenticateClient();
}

void Server::authenticateClient() {
    ClientHello hello = receiveHello();
    ServerChallenge challenge = sendChallenge(hello);

    transport::send_frame(socket_, nanopb::encode_message(ServerChallenge_fields, &challenge));
    auto buf = transport::recv_frame(socket_);
    ClientResponse resp = ClientResponse_init_zero;
    nanopb::decode_message(ClientResponse_fields, &resp, buf);

    // Verify client response
    std::string serial((char*)hello.serial_id.bytes, hello.serial_id.size);
    ByteVec challengeHash = crypto_utils::sha256(challenges[serial]);
    ByteVec sig(resp.signature.bytes, resp.signature.bytes + resp.signature.size);

    if (!crypto_utils::verify_digest(clientKeys[serial].data(), challengeHash, sig))
        throw std::runtime_error("Client auth failed");

    std::cout << "Client verified: " << serial << "\n";
}

// ------------------ Helpers ------------------
ClientHello Server::receiveHello() {
    auto buf = transport::recv_frame(socket_);
    ClientHello hello = ClientHello_init_zero;
    nanopb::decode_message(ClientHello_fields, &hello, buf);

    std::string serial((char*)hello.serial_id.bytes, hello.serial_id.size);
    ByteVec sig(hello.signature.bytes, hello.signature.bytes + hello.signature.size);
    ByteVec hash = crypto_utils::sha256(ByteVec(serial.begin(), serial.end()));

    if (!crypto_utils::verify_digest(clientKeys[serial].data(), hash, sig))
        throw std::runtime_error("ClientHello signature invalid");

    // store challenge
    challenges[serial] = crypto_utils::rand32();
    return hello;
}

ServerChallenge Server::sendChallenge(const ClientHello& hello) {
    std::string serial((char*)hello.serial_id.bytes, hello.serial_id.size);
    ByteVec& rnd = challenges[serial];

    ByteVec sig = crypto_utils::sign_digest(priv, crypto_utils::sha256(rnd));
    ServerChallenge c = ServerChallenge_init_zero;
    c.random.size = rnd.size();
    memcpy(c.random.bytes, rnd.data(), rnd.size());
    c.signature.size = sig.size();
    memcpy(c.signature.bytes, sig.data(), sig.size());
    return c;
}

// ------------------ Secure Data ------------------
void Server::sendSecure(const std::string& msg) {
    SecureData d = SecureData_init_zero;
    strncpy(d.payload, msg.c_str(), sizeof(d.payload) - 1);
    auto buf = nanopb::encode_message(SecureData_fields, &d);
    transport::send_frame(socket_, buf);
}

std::string Server::receiveSecure() {
    auto buf = transport::recv_frame(socket_);
    SecureData d = SecureData_init_zero;
    nanopb::decode_message(SecureData_fields, &d, buf);
    return std::string(std::string(d.sender) + std::string(",") +std::string(d.payload));
}
