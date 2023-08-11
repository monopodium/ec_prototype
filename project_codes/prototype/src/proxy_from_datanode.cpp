#include "proxy.h"

bool ECProject::ProxyImpl::get_from_datanode(const char *key, size_t key_length,
                                   char *value, size_t *value_length, int offset, int lenth, const char *ip, int port)
  {
    try
    {

      asio::ip::tcp::resolver resolver(m_io_context);
      asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(std::string(ip), std::to_string(port));
      asio::ip::tcp::socket socket(m_io_context);
      asio::connect(socket, endpoint);

      int flag = 1;
      std::vector<unsigned char> int_buf_flag = ECProject::int_to_bytes(flag);
      asio::write(socket, asio::buffer(int_buf_flag, int_buf_flag.size()));

      std::vector<unsigned char> int_buf_key_size = ECProject::int_to_bytes(key_length);
      asio::write(socket, asio::buffer(int_buf_key_size, int_buf_key_size.size()));

      asio::write(socket, asio::buffer(key, key_length));

      std::vector<unsigned char> int_buf_offset = ECProject::int_to_bytes(offset);
      asio::write(socket, asio::buffer(int_buf_offset, int_buf_offset.size()));

      std::vector<unsigned char> int_buf_lenth = ECProject::int_to_bytes(lenth);
      asio::write(socket, asio::buffer(int_buf_lenth, int_buf_lenth.size()));

      std::vector<unsigned char> value_from_datanode(lenth);

      asio::read(socket, asio::buffer(value_from_datanode, value_from_datanode.size()));

      asio::error_code ignore_ec;
      socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignore_ec);
      socket.close(ignore_ec);

      memcpy(value, value_from_datanode.data(), lenth);
    }
    catch (std::exception &e)
    {
      std::cout << "GetFromMemcached"
                << " " << std::string(ip) << " " << port << std::endl;
      std::cout << e.what() << std::endl;
    }

    return true;
  }