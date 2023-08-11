#ifndef DATANODE_H
#define DATANODE_H

#include "meta_definition.h"

namespace ECProject{
class DataNode
{
public:
    DataNode(std::string ip, int port,std::string sub_ip, int sub_port) : m_ip(ip), m_port(port),m_sub_ip(sub_ip), m_sub_port(sub_port),
                                         m_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::address::from_string(ip.c_str()), port))
    {
        memcached_return rc;
        m_memcached = memcached_create(NULL);
        memcached_server_st *servers;
        servers = memcached_server_list_append(NULL, sub_ip.c_str(), sub_port, &rc);
        rc = memcached_server_push(m_memcached, servers);
        memcached_server_free(servers);
        memcached_behavior_set(m_memcached, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT);
        memcached_behavior_set(m_memcached, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 20);
        memcached_behavior_set(m_memcached, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 5);
        memcached_behavior_set(m_memcached, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, true);
    }
    void start();

private:
    void do_work();
    memcached_st *m_memcached;
    std::string m_ip;
    int m_port;
    std::string m_sub_ip;
    int m_sub_port;
    asio::io_context m_io_context;
    asio::ip::tcp::acceptor m_acceptor;
};
}
#endif