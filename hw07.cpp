#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <list>
#include <memory>
#include <string>

using btcp = boost::asio::ip::tcp;

struct Connection
{
    btcp::socket socket;
    boost::asio::streambuf read_buffer;
    Connection(boost::asio::io_service &io_service) : socket(io_service), read_buffer() {}
};

class Server
{
    boost::asio::io_service ioservice;
    btcp::acceptor acceptor;
    std::list<Connection> connections;

public:
    Server(uint16_t port) : acceptor(ioservice, btcp::endpoint(btcp::v4(), port)) { start_accept(); }

    void on_read(std::list<Connection>::iterator con_handle, boost::system::error_code const &err, size_t bytes_transfered)
    {
        if (bytes_transfered > 0)
        {
            std::istream is(&con_handle->read_buffer);
            std::string line;
            std::getline(is, line);
			if (line.substr(0, 8) == "username"){
				auto buff = std::make_shared<std::string>(con_handle->socket.remote_endpoint().address().to_string() + "\n");
				auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
				boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
				do_async_read(con_handle);
			}
			if (line.substr(0, 8) == "hostname"){
				auto buff = std::make_shared<std::string>(boost::asio::ip::host_name() + "\n");
				auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
            	boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
            	do_async_read(con_handle);
			}
			if (line.substr(0, 14) == "serverdatetime"){
				boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::universal_time();
				auto buff = std::make_shared<std::string>(boost::posix_time::to_simple_string(datetime) + "\n");
				auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
            	boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
            	do_async_read(con_handle);
			}
			if (line.substr(0, 4) == "quit"){
				connections.erase(con_handle);
			}
			if (line.substr(0, 5) == "mkdir"){
				try {
    				boost::filesystem::path dest(line.substr(6));
    				boost::filesystem::create_directory(dest.root_path());
    			} catch(const boost::filesystem::filesystem_error& e) {
    				std::cerr << e.what();
  				}
				do_async_read(con_handle);
			}
			if (line.substr(0, 2) == "ls"){
				for (auto& entry : boost::filesystem::directory_iterator(line.substr(3))) {
					boost::filesystem::file_status st = entry.status();
					boost::filesystem::path p(entry);
					std::string f = p.filename().string();
					auto buff = std::make_shared<std::string>(f + "\n");
					auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
					boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
					do_async_read(con_handle);
				}
        	}
            std::cout << "Message Received: " << line << std::endl;
        }

        if (!err)
        {
            do_async_read(con_handle);
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
    }

    void do_async_read(std::list<Connection>::iterator con_handle)
    {
        auto handler = boost::bind(&Server::on_read, this, con_handle, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
        boost::asio::async_read_until(con_handle->socket, con_handle->read_buffer, "\n", handler);
    }

    void on_write(std::list<Connection>::iterator con_handle, std::shared_ptr<std::string> msg_buffer, boost::system::error_code const &err)
    {
        if (!err)
        {
            std::cout << "Finished sending message\n";
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
    }

    void on_accept(std::list<Connection>::iterator con_handle, boost::system::error_code const &err)
    {
        if (!err)
        {            
            std::cout << "Connection from: " << con_handle->socket.remote_endpoint().address().to_string() << "\n";
            std::cout << "Sending message\n";
            auto buff = std::make_shared<std::string>("Hello Client!\n\n\n");
            auto handler = boost::bind(&Server::on_write, this, con_handle, buff, boost::asio::placeholders::error);
            boost::asio::async_write(con_handle->socket, boost::asio::buffer(*buff), handler);
            do_async_read(con_handle);
        }
        else
        {
            std::cerr << "Error: " << err.message() << std::endl;
            connections.erase(con_handle);
        }
        start_accept();
    }

    void start_accept()
    {
        auto con_handle = connections.emplace(connections.begin(), ioservice);
        auto handler = boost::bind(&Server::on_accept, this, con_handle, boost::asio::placeholders::error);
        acceptor.async_accept(con_handle->socket, handler);
    }

    void run()
    {
        ioservice.run();
    }
};

int main()
{
    Server srv(13337);
    srv.run();
    return 0;
};