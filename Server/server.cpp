#include <boost/asio.hpp>
#include <nanopb/pb_encode.h>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;

        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server listening on port 8080...\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);
            // std::cout << "Client connected!" << std::endl;
            // std::string message = "Hello from server!";
            // boost::asio::write(socket, boost::asio::buffer(stream.bytes_written));
            
            // Prepare NanoPB message
            ChatMessage msg = ChatMessage_init_zero;
            msg.sender = "Server";
            msg.text = "Hello from NanoPB server!";
            if (!pb_encode(&stream, ChatMessage_fields, &msg)) {
                std::cerr << "Encoding failed!" << std::endl;
                return 1;
            }
            boost::asio::write(socket, boost::asio::buffer(stream.bytes_written));

            std::cout << "Message sent to client\n";
        }

    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
