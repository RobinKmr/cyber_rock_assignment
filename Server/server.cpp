#include "boost/asio.hpp"
#include <iostream>
#include <string>
extern "C" {
    #include "pb_common.h"
    #include "pb_encode.h"
    #include "pb_decode.h"
    #include "sensor.pb.h"
}

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io)
        : acceptor_server(io, tcp::endpoint(tcp::v4(), 8080)),
            socket_server(io),
            timer_server(io),
            counter(0) {}

    void start() {
        client_connect();
    }
private:
    void client_connect(){
        std::cout << "Server listening on port 8080...\n";

        acceptor_server.async_accept(socket_server, [this](auto ec) {
            if (!ec) {
                std::cout << "Client connected" << std::endl;
                start_receive();
                schedule_send();
            }
        });
    }

    // Receive loop
    void start_receive() {
        socket_server.async_read_some(boost::asio::buffer(rx_buffer),
            [this](auto ec, std::size_t bytes) {
                if (!ec) {
                    decode_message(bytes);
                    start_receive(); // keep reading
                } else if (ec == boost::asio::error::eof) {
                    std::cout << "Client disconnected." << std::endl;
                    timer_server.cancel();
                    socket_server.close();
                    client_connect(); 
                } else {
                    throw boost::system::system_error(ec);
                }
            }
        );
    }

    void decode_message(std::size_t bytes) {
        // Decode the message
        SensorData inmsg = SensorData_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(rx_buffer, bytes);
        if (!pb_decode(&stream, SensorData_fields, &inmsg)) {
            std::cerr << "Decode failed" << std::endl;
        }
        std::cout << "Received from " << inmsg.sender << ": " << inmsg.text << std::endl;
    }

    void schedule_send() {
        timer_server.expires_after(std::chrono::milliseconds(2000));
        timer_server.async_wait(
            [this](const boost::system::error_code& ec) {
                if (!ec) {
                    send_message();
                    schedule_send(); // re-arm timer
                }
            }
        );
    }
    
    void send_message() {
        SensorData msg = SensorData_init_zero;
        snprintf(msg.sender, sizeof(msg.sender), "Server");
        snprintf(msg.text, sizeof(msg.text), "Hello from server");
        msg.value = sizeof(msg.sender) + sizeof(msg.text);

        uint8_t buffer[256];
        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        if (!pb_encode(&ostream, SensorData_fields, &msg)) {
            std::cerr << "Encoding failed!" << PB_GET_ERROR(&ostream) << std::endl;
        }
        boost::asio::write(socket_server, boost::asio::buffer(buffer, ostream.bytes_written));
    }

    tcp::acceptor acceptor_server;
    tcp::socket socket_server;
    boost::asio::steady_timer timer_server;
    uint8_t rx_buffer[256];
    int counter;
};

int main() {
    try {
        boost::asio::io_context io;
        Server server(io);
        server.start();
        io.run();
    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
