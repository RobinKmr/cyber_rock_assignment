#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

extern "C" {
#include "bignum.h"
#include "secp256k1.h"
#include "rand.h"
#include "range.pb.h"
}

inline void ecpoint_to_pb(const curve_point &p, RangePoint &out) {
    out.x.size = 32;
    out.y.size = 32;
    memcpy(out.x.bytes, p.x.val, 32);
    memcpy(out.y.bytes, p.y.val, 32);
}

inline void pb_to_ecpoint(const RangePoint &in, curve_point &p) {
    memcpy(p.x.val, in.x.bytes, 32);
    memcpy(p.y.val, in.y.bytes, 32);
}

/* ========== SCALAR HELPERS ========== */
inline void random_scalar(bignum256 &k) {
    uint8_t buf[32];
    do {
        random_buffer(buf, sizeof(buf));
        bn_read_be(buf, &k);
        bn_mod(&k, &secp256k1.order);
    } while (bn_is_zero(&k));
}

/* simple bignum from uint64 */
inline bignum256 ec_bn_64(uint64_t x) {
    bignum256 r;
    bn_zero(&r);
    r.val[0] = (uint32_t)(x & 0xFFFFFFFF);
    r.val[1] = (uint32_t)(x >> 32);
    return r;
}

/* ========== ECC Wrapper ========== */

/* point addition: r = a + b */
inline void ec_add(curve_point &r, const curve_point &a, const curve_point &b) {
    r = a;
    point_add(&secp256k1, &b, &r);
}

/* point negation: p = -p */
inline void ec_neg(curve_point &p) {
    if (bn_is_zero(&p.y)) return;
    bignum256 res;
    bn_subtract(&secp256k1.prime, &p.y, &res);
    p.y=res;
}

/* scalar multiplication: r = k * p */
inline void ec_mul(curve_point &r, const bignum256 &k, const curve_point &p) {
    bignum256 kin = k;
    // Normalize scalar for trezor-crypto
    bn_mod(&kin, &secp256k1.order);
    if (bn_is_zero(&kin)) {
        // Map zero → 1 (safe for demo, avoids infinity)
        kin = ec_bn_64(64);
    }
    point_multiply(&secp256k1, &kin, &p, &r);
}

inline bignum256 bn_square_u64(uint64_t v) {
    bignum256 bn = ec_bn_64(v);
    bn_multiply(&bn, &bn, &bn);
    bn_mod(&bn, &secp256k1.order);
    return bn;
}


/* equality check, true if equal */
inline bool ec_equal(const curve_point &a, const curve_point &b) {
    return memcmp(&a, &b, sizeof(curve_point)) == 0;
}

/* ========== Generators ========== */
inline curve_point make_G() {
    curve_point G;
    G = secp256k1.G; ; // secp256k1 base point
    return G;
}

inline curve_point make_H() {
    curve_point H;
    bignum256 k;
    random_scalar(k);
    point_multiply(&secp256k1, &k ,&secp256k1.G , &H);
    return H;
}

/* ========== Range Proof Struct ========== */

struct RangeProof {
    curve_point C[4];
    curve_point Cx;
    curve_point CC1, CC2;
    uint64_t a, b, n;
};

/* ========== Boost.Asio Serialization ========== */

inline void send_point(boost::asio::ip::tcp::socket &s, const curve_point &p) {
    boost::asio::write(s, boost::asio::buffer(&p, sizeof(curve_point)));
}

inline void recv_point(boost::asio::ip::tcp::socket &s, curve_point &p) {
    boost::asio::read(s, boost::asio::buffer(&p, sizeof(curve_point)));
}
