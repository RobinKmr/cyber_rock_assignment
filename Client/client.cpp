#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;

        tcp::resolver resolver(io);
        tcp::socket socket(io);
        
        auto endpoints = resolver.resolve("server", "8080");
        boost::asio::connect(socket, endpoints);

        char buffer[1024];
        size_t len = socket.read_some(boost::asio::buffer(buffer));

        std::cout << "Received: " << std::string(buffer, len) << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}
