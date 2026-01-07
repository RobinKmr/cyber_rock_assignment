#include "server.hpp"
#include <crypto_utils.hpp>
#include "range_proof_utils.hpp"
#include <nanopb_utils.hpp>
#include <transport.hpp>
#include "auth.pb.h"
#include "data.pb.h"

Server::Server(tcp::socket socket, const uint8_t privKey[32])
        : socket_(std::move(socket)){
// Server::Server(boost::asio::io_context& io, uint16_t port, const uint8_t privKey[32])
//     : acceptor_(io, tcp::endpoint(tcp::v4(), port)), socket_(io) {
    memcpy(priv, privKey, 32);
}

void Server::registerClient(const std::string& serial, const uint8_t pub[64]) {
    clientKeys[serial] = ByteVec(pub, pub + 64);
}

void Server::run() {
    // acceptor_.accept(socket_);
    authenticateClient();
}

void Server::authenticateClient() {
    ClientHello hello = receiveHello();
    ServerChallenge challenge = sendChallenge(hello);

    transport::send_frame(socket_, nanopb::encode_message(ServerChallenge_fields, &challenge));
    ClientResponse resp = ClientResponse_init_zero;
    nanopb::decode_message(ClientResponse_fields, &resp, transport::recv_frame(socket_));

    // Verify client response
    std::string serial((char*)hello.serial_id.bytes, hello.serial_id.size);
    ByteVec challengeHash = crypto_utils::sha256(challenges[serial]);
    ByteVec sig(resp.signature.bytes, resp.signature.bytes + resp.signature.size);

    if (!crypto_utils::verify_digest(clientKeys[serial].data(), challengeHash, sig))
        throw std::runtime_error("Client auth failed");

    std::cout << "Client verified: " << serial << "\n";
}

void Server::rangeProofClient(){
    RangeProofRequest req = RangeProofRequest_init_default;
    nanopb::decode_message(RangeProofRequest_fields, &req, transport::recv_frame(socket_));
    
    curve_point G = make_G();
    // P1 = bG − CC1
    curve_point bG, negCC1, P1;
    pb_to_ecpoint(req.CC1, negCC1);
    ec_mul(bG, ec_bn_64(req.b), G);
    ec_neg(negCC1);
    ec_add(P1, bG, negCC1);
    
    // P2 = CC2 + aG
    curve_point aG, P2, CC2;
    pb_to_ecpoint(req.CC1, CC2);
    ec_mul(aG, ec_bn_64(req.a), G);
    ec_add(P2, CC2, aG);

    bool ok = ec_equal(P1, P2);
    if(!ok){
        std::runtime_error("[FAIL] Range verification");
    }
    RangeProofResponse resp = RangeProofResponse_init_default;
    resp.success = ok;
    transport::send_frame(socket_, nanopb::encode_message(RangeProofResponse_fields, &resp));
}

// ------------------ Helpers ------------------
ClientHello Server::receiveHello() {
    ClientHello hello = ClientHello_init_zero;
    nanopb::decode_message(ClientHello_fields, &hello, transport::recv_frame(socket_));

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
void Server::sendSecure(const std::string& sender, const std::string& payload) {
    SecureData d = SecureData_init_zero;
    strncpy(d.sender, sender.c_str(), sizeof(d.sender) - 1);
    strncpy(d.payload, payload.c_str(), sizeof(d.payload) - 1);
    transport::send_frame(socket_, nanopb::encode_message(SecureData_fields, &d));
}

SecureData Server::receiveSecure() {
    SecureData d = SecureData_init_zero;
    nanopb::decode_message(SecureData_fields, &d, transport::recv_frame(socket_));
    return d;
}
