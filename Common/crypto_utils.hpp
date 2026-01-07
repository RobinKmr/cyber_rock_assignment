#pragma once

extern "C" {
#include <bignum.h>
#include <ecdsa.h>
#include <secp256k1.h>
#include <sha2.h>
#include <rand.h>
}

using ByteVec = std::vector<uint8_t>;

namespace crypto_utils {

inline ByteVec sha256(const ByteVec& data) {
    ByteVec h(32);
    sha256_Raw(data.data(), data.size(), h.data());
    return h;
}

inline ByteVec rand32() {
    ByteVec r(32);
    random_buffer(r.data(), 32);
    return r;
}

inline void derive_pubkey(const uint8_t priv[32], uint8_t pub[64]) {
    uint8_t tmp[65];
    ecdsa_get_public_key65(&secp256k1, priv, tmp);
    memcpy(pub, tmp + 1, 64);
}

inline ByteVec sign_digest(const uint8_t priv[32], const ByteVec& hash) {
    ByteVec sig(64);
    ecdsa_sign_digest(&secp256k1, priv, hash.data(), sig.data(), nullptr, nullptr);
    return sig;
}

inline bool verify_digest(const uint8_t pub[64], const ByteVec& hash, const ByteVec& sig) {
    return ecdsa_verify_digest(&secp256k1, pub, sig.data(), hash.data());
}

} // namespace crypto_utils
