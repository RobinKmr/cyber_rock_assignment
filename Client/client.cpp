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

class Client {
public:
    Client(boost::asio::io_context& io)
    : socket_client(io),
        resolver_client(io),
        timer_client(io),
        counter(0) {}

        
    void start() {
        auto endpoints = resolver_client.resolve(
            tcp::v4(),
            "127.0.0.1",
            "8080",
            tcp::resolver::numeric_host |
            tcp::resolver::numeric_service
        );

        boost::asio::connect(socket_client, endpoints);
        std::cout << "Connected to server." << std::endl;
        schedule_send();
        start_receive();
    }

private:
    void schedule_send() {
        timer_client.expires_after(std::chrono::milliseconds(1000));
        timer_client.async_wait(
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
        snprintf(msg.sender, sizeof(msg.sender), "Client");
        snprintf(msg.text, sizeof(msg.text), "ping to server");
        msg.value = sizeof(msg.sender) + sizeof(msg.text);
        uint8_t buffer[256];
        pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        if (!pb_encode(&ostream, SensorData_fields, &msg)) {
            std::cerr << "Encoding failed!" << PB_GET_ERROR(&ostream) << std::endl;
        }
        boost::asio::write(socket_client, boost::asio::buffer(buffer, ostream.bytes_written));
    }

    void start_receive(){
        socket_client.async_read_some(boost::asio::buffer(rx_buffer),
            [this](auto ec, std::size_t bytes) {
                if (!ec) {
                    decode_message(bytes);
                    start_receive(); // keep listening
                }
            }
        );
    }

    void decode_message(std::size_t bytes){
        SensorData inmsg = SensorData_init_zero;
        pb_istream_t stream = pb_istream_from_buffer(rx_buffer, bytes);
        if (!pb_decode(&stream, SensorData_fields, &inmsg)) {
            std::cerr << "Decoding failed!" << std::endl;
        }
        std::cout << "Message from " << inmsg.sender << ": " << inmsg.text << std::endl;
    }

    tcp::socket socket_client;
    tcp::resolver resolver_client;
    boost::asio::steady_timer timer_client;
    uint8_t rx_buffer[1024];
    int counter;
};

int main() {
    try {
        boost::asio::io_context io;
        Client client(io);
        client.start();
        io.run();
    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
    return 0;
}
