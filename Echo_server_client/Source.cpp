#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <fstream>
#include <string>

using namespace boost::asio;
using ip::tcp;


class con_handler : public std::enable_shared_from_this<con_handler>
{
private:
	tcp::socket sock;
	std::string message = "";
	std::string message2 = "000010";
	std::string message_to_send = "";
	static const int max_length = 10 * 1024;
	char data[max_length] = { 0 };

public:

	typedef std::shared_ptr<con_handler> pointer;
	con_handler(boost::asio::io_context& io_context)
		: sock(io_context)
	{
	}

	void set_message_from_file(std::string filename)
	{
		std::string line;
		std::ifstream myfile(filename);
		if (myfile.is_open())
		{
			while (getline(myfile, line)) {
				this->message += line + "\n";
				//out << line << endl;
			}
			this->message += "\n";
			myfile.close();
		}
		else
		{
			std::cout << "Unable to open file";
		}
	}

	static pointer create(boost::asio::io_context& io_context)
	{
		return std::make_shared<con_handler>(std::move(con_handler(io_context)));
	}

	tcp::socket& get_socket()
	{
		return sock;
	}

	void start()
	{
		std::cout << "New connection with client on address: " << sock.remote_endpoint() << std::endl;
		sock.async_read_some(boost::asio::buffer(data, max_length),
			boost::bind(&con_handler::handle_read,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

		std::string var = data;
		if (var == "attack")
		{
			message_to_send = message2;

			std::cout << "message type of attack" << std::endl;
		}
		else
		{
			this->message_to_send = message;
			std::cout << "Client wants to update his neural network!" << std::endl;
		}

		sock.async_write_some(boost::asio::buffer(message_to_send, max_length),
			boost::bind(&con_handler::handle_write,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_read(const boost::system::error_code& err,
		size_t bytes_transferred)
	{
		if (!err) {

			std::cout << data << std::endl;
		}
		else {
			std::cerr << "err (recv): " << err.message() << std::endl;
			sock.close();
		}
	}
	void handle_write(const boost::system::error_code& err,
		size_t bytes_transferred)
	{
		if (!err) {

			std::cout << "Server sent Hello message!" << std::endl;
			std::cout << "Message sent:" << message_to_send << std::endl;

		}
		else {
			std::cerr << "err (recv): " << err.message() << std::endl;
			sock.close();
			std::cout << "Connection with client on address: " << sock.remote_endpoint() << "ended!" << std::endl;
		}
	}

};

class Server {

private:
	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor;
	std::string filename;

	void start_accept()
	{
		std::cout << "Starting a server!" << std::endl;
		std::cout << "Listening on address: " << acceptor.local_endpoint().address()
			<< std::endl << "Listening on port:" << acceptor.local_endpoint().port();
		auto connection = con_handler::create(io_context_);
		connection->set_message_from_file(filename);

		acceptor.async_accept(connection->get_socket(),
			boost::bind(&Server::handle_accept, this, connection,
				boost::asio::placeholders::error));
	}
public:
	Server(boost::asio::io_context& io_context, std::uint16_t port, std::string&& filename) : io_context_(io_context), acceptor(io_context_, tcp::endpoint(tcp::v4(), port)), filename(std::move(filename))
	{
		start_accept();
	}


	void handle_accept(con_handler::pointer connection,
		const boost::system::error_code& err)
	{
		if (!err) {
			connection->start();
		}
		start_accept();
	}

};

int main(int argc, char* argv[])
{
	bool running = true;
	std::string user_input = "";
	try
	{
		boost::asio::io_context io_context;
		Server server(io_context, 1234, "nn.txt");
		io_context.run();
		std::cout << "curaci";
	}

	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}


	while (running)
	{
		std::cin >> user_input;

	}


	return 0;
}
