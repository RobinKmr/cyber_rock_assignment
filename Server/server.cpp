#include <boost/asio.hpp>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;

        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server listening on port 8080...\n";

        tcp::socket socket(io);
        acceptor.accept(socket);

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            std::string message = "Hello from server!";
            boost::asio::write(socket, boost::asio::buffer(message));

            std::cout << "Message sent to client\n";
        }

    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
