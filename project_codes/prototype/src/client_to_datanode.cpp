#include "client.h"

bool ECProject::Client::set_to_datanode(const char *key, size_t key_length,
                                 const char *value, size_t value_length, const char *ip, int port)
  {
    try
    {

      asio::ip::tcp::resolver resolver(m_io_context);
      asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(std::string(ip), std::to_string(port));
      asio::ip::tcp::socket socket(m_io_context);
      asio::connect(socket, endpoint);

      int flag = 0;
      std::vector<unsigned char> int_buf_flag = ECProject::int_to_bytes(flag);
      asio::write(socket, asio::buffer(int_buf_flag, int_buf_flag.size()));

      std::vector<unsigned char> int_buf_key_size = ECProject::int_to_bytes(key_length);
      asio::write(socket, asio::buffer(int_buf_key_size, int_buf_key_size.size()));

      std::vector<unsigned char> int_buf_value_size = ECProject::int_to_bytes(value_length);
      asio::write(socket, asio::buffer(int_buf_value_size, int_buf_value_size.size()));

      asio::write(socket, asio::buffer(key, key_length));
      asio::write(socket, asio::buffer(value, value_length));

      std::vector<char> finish(1);
      asio::read(socket, asio::buffer(finish, finish.size()));

      asio::error_code ignore_ec;
      socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignore_ec);
      socket.close(ignore_ec);
    }
    catch (std::exception &e)
    {
      std::cout << "client set to datanode"
                << " " << std::string(ip) << " " << port << std::endl;
      std::cout << e.what() << std::endl;
    }

    // std::cout << "set " << std::string(key) << " " << std::string(ip) << " " << port << " " << (MEMCACHED_SUCCESS==rc) << " " << value_length << std::endl;
    return true;
  }