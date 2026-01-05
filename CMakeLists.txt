cmake_minimum_required(VERSION 3.16)
project(cyber_rock_assignment LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Common ----
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/Common)
set(COMMON_SRC
    ${COMMON_DIR}/crypto_utils.hpp
    ${COMMON_DIR}/nanopb_utils.hpp
    ${COMMON_DIR}/transport.hpp
)
# ---- Boost ----
# add_subdirectory(${CMAKE_SOURCE_DIR}/libs/boost)
find_package(Boost REQUIRED COMPONENTS system)

# ---- Nanopb paths ----
set(NANOPB_DIR ${CMAKE_SOURCE_DIR}/libs/nanopb)
set(NANOPB_SRC
    ${NANOPB_DIR}/pb_encode.c
    ${NANOPB_DIR}/pb_decode.c
    ${NANOPB_DIR}/pb_common.c
)

# ---- Shared protobuf sources ----
set(PROTO_DIR ${CMAKE_SOURCE_DIR}/generated/proto)
set(PROTO_SRC
    ${PROTO_DIR}/data.pb.c
    ${PROTO_DIR}/auth.pb.c
)

# ---- Trezor crypto paths ----
set(TREZOR_CRYPTO_DIR ${CMAKE_SOURCE_DIR}/libs/trezor-crypto)
set(TREZOR_CRYPTO_SRC
    ${TREZOR_CRYPTO_DIR}/bip39.c
    ${TREZOR_CRYPTO_DIR}/hasher.c
    # ${TREZOR_CRYPTO_DIR}/chacha20poly1305/chacha_merged.c
    # ${TREZOR_CRYPTO_DIR}/chacha20poly1305/poly1305-donna.c
    # ${TREZOR_CRYPTO_DIR}/chacha20poly1305/rfc7539.c
    # ${TREZOR_CRYPTO_DIR}/chacha20poly1305/chacha20poly1305.c
    ${TREZOR_CRYPTO_DIR}/bignum.c
    ${TREZOR_CRYPTO_DIR}/sha3.c
    # ${TREZOR_CRYPTO_DIR}/nem.c
    # ${TREZOR_CRYPTO_DIR}/bip32.c
    ${TREZOR_CRYPTO_DIR}/groestl.c
    ${TREZOR_CRYPTO_DIR}/memzero.c
    ${TREZOR_CRYPTO_DIR}/shamir.c
    ${TREZOR_CRYPTO_DIR}/slip39.c
    ${TREZOR_CRYPTO_DIR}/rc4.c
    ${TREZOR_CRYPTO_DIR}/nist256p1.c
    ${TREZOR_CRYPTO_DIR}/rand.c
    ${TREZOR_CRYPTO_DIR}/blake2s.c
    ${TREZOR_CRYPTO_DIR}/segwit_addr.c
    # ${TREZOR_CRYPTO_DIR}/monero/serialize.c
    # ${TREZOR_CRYPTO_DIR}/monero/base58.c
    # ${TREZOR_CRYPTO_DIR}/monero/range_proof.c
    # ${TREZOR_CRYPTO_DIR}/monero/xmr.c
    # ${TREZOR_CRYPTO_DIR}/tests/test_check.c
    # ${TREZOR_CRYPTO_DIR}/tests/test_speed.c
    # ${TREZOR_CRYPTO_DIR}/tests/test_openssl.c
    ${TREZOR_CRYPTO_DIR}/sha2.c
    ${TREZOR_CRYPTO_DIR}/base58.c
    ${TREZOR_CRYPTO_DIR}/rfc6979.c
    # ${TREZOR_CRYPTO_DIR}/tools/xpubaddrgen.c
    # ${TREZOR_CRYPTO_DIR}/tools/bip39bruteforce.c
    # ${TREZOR_CRYPTO_DIR}/tools/mktable.c
    ${TREZOR_CRYPTO_DIR}/ripemd160.c
    ${TREZOR_CRYPTO_DIR}/hmac_drbg.c
    ${TREZOR_CRYPTO_DIR}/base32.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519-donna-impl-base.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/curve25519-donna-32bit.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519-keccak.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/modm-donna-32bit.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519-sha3.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519-donna-32bit-tables.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/curve25519-donna-helpers.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/curve25519-donna-scalarmult-base.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519-donna-basepoint-table.c
    # ${TREZOR_CRYPTO_DIR}/ed25519-donna/ed25519.c
    ${TREZOR_CRYPTO_DIR}/script.c
    ${TREZOR_CRYPTO_DIR}/secp256k1.c
    ${TREZOR_CRYPTO_DIR}/cash_addr.c
    ${TREZOR_CRYPTO_DIR}/pbkdf2.c
    ${TREZOR_CRYPTO_DIR}/blake2b.c
    # ${TREZOR_CRYPTO_DIR}/aes/aestst.c
    # ${TREZOR_CRYPTO_DIR}/aes/aeskey.c
    # ${TREZOR_CRYPTO_DIR}/aes/aestab.c
    # ${TREZOR_CRYPTO_DIR}/aes/aescrypt.c
    # ${TREZOR_CRYPTO_DIR}/aes/aes_modes.c
    ${TREZOR_CRYPTO_DIR}/curves.c
    ${TREZOR_CRYPTO_DIR}/ecdsa.c
    ${TREZOR_CRYPTO_DIR}/hmac.c
    ${TREZOR_CRYPTO_DIR}/address.c
    ${TREZOR_CRYPTO_DIR}/blake256.c
)


include_directories(
    ${Boost_INCLUDE_DIRS}
    ${NANOPB_DIR}
    ${TREZOR_CRYPTO_DIR}
    ${PROTO_DIR}
    ${COMMON_DIR}
    )
    
    # file(GLOB_RECURSE all_sources "*.cpp")
    # ---- Server ----
    add_executable(server
    Server/server.cpp
    Server/main.cpp
    ${PROTO_SRC}
    ${NANOPB_SRC}
    ${TREZOR_CRYPTO_SRC}
    ${COMMON_SRC}
    )

# ---- Client ----
add_executable(client
    Client/client.cpp
    Client/main.cpp
    ${PROTO_SRC}
    ${NANOPB_SRC}
    ${TREZOR_CRYPTO_SRC}
    ${COMMON_SRC}
)
