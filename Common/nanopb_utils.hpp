#pragma once

#include <pb_encode.h>
#include <pb_decode.h>
#include <vector>
#include <stdexcept>

namespace nanopb {

inline std::vector<uint8_t> encode_message(const pb_msgdesc_t* fields, const void* msg) {
    uint8_t buffer[512];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (!pb_encode(&stream, fields, msg))
        throw std::runtime_error("Nanopb encode failed");
    return {buffer, buffer + stream.bytes_written};
}

inline void decode_message(const pb_msgdesc_t* fields, void* msg, const std::vector<uint8_t>& buf) {
    pb_istream_t stream = pb_istream_from_buffer(buf.data(), buf.size());
    if (!pb_decode(&stream, fields, msg))
        throw std::runtime_error("Nanopb decode failed");
}

} // namespace nanopb
