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

        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Server listening on port 8080...\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);
            std::cout << "Client connected!" << std::endl;

            while (true){
                // Create inbuffer for decoding data
                uint8_t buffer[256];
                boost::system::error_code error;
                size_t length = socket.read_some(boost::asio::buffer(buffer), error);

                if (error == boost::asio::error::eof) {
                    std::cout << "Client disconnected." << std::endl;
                    break;
                } else if (error) {
                    throw boost::system::system_error(error);
                }

                // Decode the message
                SensorData inmsg = SensorData_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(buffer, length);
                if (!pb_decode(&stream, SensorData_fields, &inmsg)) {
                    std::cerr << "Decode failed" << std::endl;
                    continue;
                }
                std::cout << inmsg.sender << ": " << inmsg.text << std::endl;
                
                // Prepare NanoPB message
                SensorData msg = SensorData_init_zero;
                snprintf(msg.sender, sizeof(msg.sender), "Server");
                snprintf(msg.text, sizeof(msg.text), "Hello from server");
                msg.value = sizeof(msg.sender) + sizeof(msg.text);
                
                // Create outbuffer for encoded data
                uint8_t outbuf[256];
                pb_ostream_t ostream = pb_ostream_from_buffer(outbuf, sizeof(outbuf));
                if (!pb_encode(&ostream, SensorData_fields, &msg)) {
                    std::cerr << "Encoding failed!" << PB_GET_ERROR(&ostream) << std::endl;
                    return 1;
                }
                boost::asio::write(socket, boost::asio::buffer(outbuf, ostream.bytes_written));
    
                std::cout << "Sent " << ostream.bytes_written << " bytes." << std::endl;
            }
        }

    } catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}
