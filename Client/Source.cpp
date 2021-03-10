#include <iostream>
#include <boost/asio.hpp>
#include <fstream>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;


class Client
{
private:
    std::string filename;
    boost::asio::io_service& io_service;
    boost::asio::ip::address server_ip;
    std::uint16_t server_port;
    tcp::socket socket;
    std::string msg_from_client = "Update my network please!\n";

public:
    Client(std::string filename, std::string ip_address, std::uint16_t port, boost::asio::io_service& io) :
        filename(filename), server_ip(boost::asio::ip::address::from_string(ip_address)), server_port(port), io_service(io),socket(io_service)
    {
        socket.connect(tcp::endpoint(server_ip, server_port));
    }

    void write_to_server()
    {
        boost::system::error_code err;
        boost::asio::write(socket, boost::asio::buffer(msg_from_client), err);
        if (!err)
        {
            std::cout << "Message sent succesfully!" << std::endl;
        }
        else
        {
            cout << "send failed, error message:" << err.message() << endl;
        }

    }

    void read_from_server()
    {
        boost::system::error_code err;
        boost::asio::streambuf receive_buffer;

        boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), err);

        std::ofstream output_file(filename);

        if (!output_file)
        {
            std::cout << "error opening file";
        }
        output_file.clear();

        if (err && err != boost::asio::error::eof) {
            cout << "Listening failed, error message: " << err.message() << endl;
        }
        else {
            std::string data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
            // const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
            cout << data << endl;
            output_file << data;
        }
        output_file.close();
    }
};



int main() {
    boost::asio::io_service io_service;

    Client c("nn.txt", "127.0.0.1", 1234, io_service);
    c.write_to_server();
    c.read_from_server();


    return 0;
}
