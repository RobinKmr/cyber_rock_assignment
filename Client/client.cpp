#include "boost/asio.hpp"
#include <iostream>
extern "C" {
    #include "pb_common.h"
    #include "pb_encode.h"
    #include "pb_decode.h"
    #include "sensor.pb.h"
}

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;
        tcp::resolver resolver(io);
        
        auto endpoints = resolver.resolve(
            tcp::v4(),
            "127.0.0.1", 
            "8080",
            tcp::resolver::numeric_host |
            tcp::resolver::numeric_service);
        tcp::socket socket(io);
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected to server." << std::endl;
        
        while(true){
            std::string input;
            std::cout << "Enter message: " << std::endl;
            // std::getline(std::cin, input);
            // if (input == "exit") break;

            // Encode
            SensorData msg = SensorData_init_zero;
            snprintf(msg.sender, sizeof(msg.sender), "Client");
            snprintf(msg.text, sizeof(msg.text), "ping to server");
            msg.value = sizeof(msg.sender) + sizeof(msg.text);

            uint8_t buffer[256];
            pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
            if (!pb_encode(&ostream, SensorData_fields, &msg)) {
                std::cerr << "Encoding failed!" << PB_GET_ERROR(&ostream) << std::endl;
                return 1;
            }
            boost::asio::write(socket, boost::asio::buffer(buffer, ostream.bytes_written));
            
            uint8_t inbuffer[1024];
            size_t len = socket.read_some(boost::asio::buffer(inbuffer));
            SensorData inmsg = SensorData_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(inbuffer, len);
            if (!pb_decode(&stream, SensorData_fields, &inmsg)) {
                std::cerr << "Decoding failed!" << std::endl;
                return 1;
            }
            std::cout << "Message from " << inmsg.sender << ": " << inmsg.text << std::endl;
        }

    } catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
    return 0;
}
