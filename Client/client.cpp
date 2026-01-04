#include <boost/asio.hpp>
#include <nanopb/pb_decode.h>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;
        tcp::resolver resolver(io);
        
        auto endpoints = resolver.resolve("server", "8080");
        tcp::socket socket(io);
        boost::asio::connect(socket, endpoints);

        // std::cout << "Connected to server." << std::endl;
        
        char buffer[1024];
        size_t len = socket.read_some(boost::asio::buffer(buffer));

        // std::cout << "Received: " << std::string(buffer, len) << std::endl;

        ChatMessage msg = ChatMessage_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(buffer, len);
        if (!pb_decode(&stream, ChatMessage_fields, &msg)) {
            std::cerr << "Decoding failed!" << std::endl;
            return 1;
        }
        std::cout << "Message from " << msg.sender << ": " << msg.text << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}
