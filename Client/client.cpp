#include "client.hpp"
#include <crypto_utils.hpp>
#include <nanopb_utils.hpp>
#include <transport.hpp>
#include "auth.pb.h"
#include "data.pb.h"

Client::Client(boost::asio::io_context& io,
               const std::string& serialId,
               const uint8_t privKey[32],
               const uint8_t serverPubKey[64],
               const std::string& host,
               uint16_t port)
    : socket_(io), serial(serialId) {
    memcpy(priv, privKey, 32);
    memcpy(serverPub, serverPubKey, 64);
    socket_.connect({boost::asio::ip::address::from_string(host), port});
}

void Client::authenticate() {
    ClientHello hello = createHello();
    transport::send_frame(socket_, nanopb::encode_message(ClientHello_fields, &hello));

    auto buf = transport::recv_frame(socket_);
    ServerChallenge ch = ServerChallenge_init_zero;
    nanopb::decode_message(ServerChallenge_fields, &ch, buf);

    ClientResponse resp = respondToChallenge(ch);
    transport::send_frame(socket_, nanopb::encode_message(ClientResponse_fields, &resp));
}

// ------------------ Helpers ------------------
ClientHello Client::createHello() {
    ClientHello h = ClientHello_init_zero;
    ByteVec hash = crypto_utils::sha256(ByteVec(serial.begin(), serial.end()));
    ByteVec sig = crypto_utils::sign_digest(priv, hash);

    memcpy(h.serial_id.bytes, serial.data(), serial.size());
    h.serial_id.size = serial.size();

    memcpy(h.signature.bytes, sig.data(), sig.size());
    h.signature.size = sig.size();
    return h;
}

ClientResponse Client::respondToChallenge(const ServerChallenge& ch) {
    ByteVec rnd(ch.random.bytes, ch.random.bytes + ch.random.size);
    ByteVec sig(ch.signature.bytes, ch.signature.bytes + ch.signature.size);

    if (!crypto_utils::verify_digest(serverPub, crypto_utils::sha256(rnd), sig))
        throw std::runtime_error("Server signature invalid");

    ByteVec respSig = crypto_utils::sign_digest(priv, crypto_utils::sha256(rnd));
    ClientResponse r = ClientResponse_init_zero;
    memcpy(r.signature.bytes, respSig.data(), respSig.size());
    r.signature.size = respSig.size();
    return r;
}

// ------------------ Secure Data ------------------
void Client::sendSecure(const std::string& sender, const std::string& payload) {
    SecureData d = SecureData_init_zero;
    strncpy(d.sender, sender.c_str(), sizeof(d.sender) - 1);
    strncpy(d.payload, payload.c_str(), sizeof(d.payload) - 1);
    transport::send_frame(socket_, nanopb::encode_message(SecureData_fields, &d));
}

std::string Client::receiveSecure() {
    auto buf = transport::recv_frame(socket_);
    SecureData d = SecureData_init_zero;
    nanopb::decode_message(SecureData_fields, &d, buf);
    return std::string(d.payload);
}
