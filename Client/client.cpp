#include "client.hpp"
#include <crypto_utils.hpp>
#include <nanopb_utils.hpp>
#include <range_proof_utils.hpp>
#include <transport.hpp>
#include "auth.pb.h"
#include "data.pb.h"
#include "range.pb.h"
#include "math.h"

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

    ServerChallenge ch = ServerChallenge_init_zero;
    nanopb::decode_message(ServerChallenge_fields, &ch, transport::recv_frame(socket_));

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

// Function to find n non-negative integers whose squares sum to 'target'
void find_squares(uint64_t target, uint64_t *roots, uint8_t n) {
    uint64_t remaining_n = target;    
    for(uint8_t i = 0; i < n; i++){
        roots[i] = static_cast<uint64_t>(std::sqrt(remaining_n));
        remaining_n = remaining_n - roots[i] * roots[i];
    }
    if(remaining_n){
        std::runtime_error("[FAILED] sum of all roots not match");
    }
}

/* ================= PROOF GENERATION ================= */

void Client::generate_proof() {
    curve_point G = make_G();
    curve_point H = make_H();
    RangeProof p;
    p.a = A; p.b = B; p.n = N;

    uint64_t x_roots[NOF_ROOTS] = {0};
    find_squares((uint64_t)HIDDEN_NO, x_roots, NOF_ROOTS);

    bignum256 r[NOF_ROOTS];
    bignum256 r_sum;
    bn_zero(&r_sum);
    for (int i = 0; i < NOF_ROOTS; i++) {
        random_scalar(r[i]);
        bn_mod(&r[i], &secp256k1.order);
        bn_addmod(&r_sum, &r[i], &secp256k1.order);
    }
    
    curve_point t1, t2;
    for (int i = 0; i < NOF_ROOTS; i++) {
        /* Ci = xi²·G + ri·H */
        ec_mul(t1, bn_square_u64(x_roots[i]), G);
        ec_mul(t2, r[i], H);
        ec_add(p.C[i], t1, t2);
    }
    
    /* Cx = x·G + r·H */
    ec_mul(t1, ec_bn_64(HIDDEN_NO), G);
    ec_mul(t2, r_sum, H);
    ec_add(p.Cx, t1, t2);

    /* Enforce ΣCi = Cx */
    curve_point Csum = p.C[0];
    ec_add(Csum, Csum, p.C[1]);
    ec_add(Csum, Csum, p.C[2]);
    ec_add(Csum, Csum, p.C[3]);

    if (!ec_equal(Csum, p.Cx)) {
        std::runtime_error("[FAIL] Square-sum check");
    }

    // CC1 = (b - x)·G + r1·H
    ec_mul(t1, ec_bn_64(B - HIDDEN_NO), G);
    ec_mul(t2, r_sum, H);
    ec_neg(t2);
    ec_add(p.CC1, t1, t2);

    // CC2 = (x - a)·G + r2·H
    ec_mul(t1, ec_bn_64(HIDDEN_NO - A), G);
    ec_mul(t2, r_sum, H);
    ec_add(p.CC2, t1, t2);

    RangeProofRequest req = RangeProofRequest_init_default;
    req.n = N;
    req.a = A;
    req.b = B;
    ecpoint_to_pb(p.CC1, req.CC1);
    ecpoint_to_pb(p.CC2, req.CC2);
    transport::send_frame(socket_, nanopb::encode_message(RangeProofRequest_fields, &req));

    RangeProofResponse resp = RangeProofResponse_init_default;
    nanopb::decode_message(RangeProofResponse_fields, &resp, transport::recv_frame(socket_));

    if(!resp.success){
        std::runtime_error("[FAIL] Range verification");
    }
    std::cout<<"[SUCCESS] Range verification"<<std::endl;
}

// ------------------ Secure Data ------------------
void Client::sendSecure(const std::string& sender, const std::string& payload) {
    SecureData d = SecureData_init_zero;
    strncpy(d.sender, sender.c_str(), sizeof(d.sender) - 1);
    strncpy(d.payload, payload.c_str(), sizeof(d.payload) - 1);
    transport::send_frame(socket_, nanopb::encode_message(SecureData_fields, &d));
}

SecureData Client::receiveSecure() {
    SecureData d = SecureData_init_zero;
    nanopb::decode_message(SecureData_fields, &d, transport::recv_frame(socket_));
    return d;
}
