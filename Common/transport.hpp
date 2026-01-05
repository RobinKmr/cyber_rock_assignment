#pragma once

#include <vector>
#include <boost/asio.hpp>
#include <cstdint>
#include <arpa/inet.h> // For htonl/ntohl

using boost::asio::ip::tcp;

namespace transport {

inline void send_frame(tcp::socket& sock, const std::vector<uint8_t>& buf) {
    uint32_t len = htonl(buf.size());
    boost::asio::write(sock, boost::asio::buffer(&len, sizeof(len)));
    boost::asio::write(sock, boost::asio::buffer(buf));
}

inline std::vector<uint8_t> recv_frame(tcp::socket& sock) {
    uint32_t len;
    boost::asio::read(sock, boost::asio::buffer(&len, sizeof(len)));
    len = ntohl(len);
    std::vector<uint8_t> buf(len);
    boost::asio::read(sock, boost::asio::buffer(buf));
    return buf;
}

} // namespace transport

