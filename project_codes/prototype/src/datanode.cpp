#include "datanode.h"


void ECProject::DataNode::do_work()
{
    for (;;)
    {
        asio::ip::tcp::socket socket(m_io_context);
        m_acceptor.accept(socket);
        std::vector<unsigned char> int_buf_flag(sizeof(int));
        asio::read(socket, asio::buffer(int_buf_flag, int_buf_flag.size()));
        int flag = ECProject::bytes_to_int(int_buf_flag);
        if (flag == 0)
        {
            try
            {
                std::vector<unsigned char> int_buf(sizeof(int));
                asio::read(socket, asio::buffer(int_buf, int_buf.size()));
                int key_size = ECProject::bytes_to_int(int_buf);
                asio::read(socket, asio::buffer(int_buf, int_buf.size()));
                int value_size = ECProject::bytes_to_int(int_buf);

                std::vector<unsigned char> key_buf(key_size);
                std::vector<unsigned char> value_buf(value_size);
                asio::read(socket, asio::buffer(key_buf, key_buf.size()));
                asio::read(socket, asio::buffer(value_buf, value_buf.size()));

                memcached_return_t ret = memcached_set(m_memcached, (const char *)key_buf.data(), key_buf.size(), (const char *)value_buf.data(), value_buf.size(), (time_t)0, (uint32_t)0);
                if (memcached_failed(ret))
                {
                    std::cout << "memcached_set fail" << std::endl;
                    printf("%d %s %d %d\n", key_size, std::string((char *)key_buf.data(), key_size).c_str(), m_port, ret);
                }
                std::vector<char> finish(1);
                asio::write(socket, asio::buffer(finish, finish.size()));
                asio::error_code ignore_ec;
                socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignore_ec);
                socket.close(ignore_ec);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
                exit(-1);
            }
        }
        else if (flag == 1)
        {
            try
            {
                std::vector<unsigned char> int_buf(sizeof(int));
                asio::read(socket, asio::buffer(int_buf, int_buf.size()));
                int key_size = ECProject::bytes_to_int(int_buf);

                std::vector<unsigned char> key_buf(key_size);
                asio::read(socket, asio::buffer(key_buf, key_buf.size()));

                asio::read(socket, asio::buffer(int_buf, int_buf.size()));
                int offset = ECProject::bytes_to_int(int_buf);

                asio::read(socket, asio::buffer(int_buf, int_buf.size()));
                int lenth = ECProject::bytes_to_int(int_buf);
                
                
                memcached_return_t error;
                uint32_t flag;
                size_t value_size;
                char *value_ptr = memcached_get(m_memcached, (const char *)key_buf.data(), key_buf.size(), &value_size, &flag, &error);
                if (value_ptr == NULL)
                {
                    std::cout << "memcached_get fail" << std::endl;
                    printf("%d %s %d %d\n", key_size, std::string((char *)key_buf.data(), key_size).c_str(), m_port, error);
                }
                
                asio::write(socket, asio::buffer(value_ptr + offset, lenth));
                asio::error_code ignore_ec;
                socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignore_ec);
                socket.close(ignore_ec);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
                exit(-1);
            }
        }
    }
}

void ECProject::DataNode::start()
{
    std::thread work(std::bind(&DataNode::do_work, this));
    work.join();
}