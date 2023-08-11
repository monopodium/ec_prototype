#ifndef CLIENT_H
#define CLIENT_H
#include "meta_definition.h"
namespace ECProject
{
  class Client
  {
  public:
    Client(std::string client_ip, int client_port) : m_clientip_for_get(client_ip)
                                                  ,m_clientport_for_get(client_port)
                                                  ,m_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::address::from_string(m_clientip_for_get), m_clientport_for_get))
    {
    }
    bool set(std::string key, std::string value);
    bool set_parameter_grpc(ECSchema input_ecschema);
    bool set_coordinator(std::string CoordinatorIpPort);
    std::string get(std::string key);
    bool parse_command(int argc, char **argv);
  private:
    std::unique_ptr<coordinator_proto::CoordinatorService::Stub> m_coordinator_ptr;
    std::string m_coordinator_ip_port;
    std::string m_clientip_for_get;
    int m_clientport_for_get;
    asio::io_context m_io_context;
    asio::ip::tcp::acceptor m_acceptor;
    ECSchema m_encode_parameter;
    std::shared_ptr<Code_Placement> m_encoder;
    bool set_to_datanode(const char *key, size_t key_length,
                                    const char *value, size_t value_length, const char *ip, int port);
    bool get_from_datanode(const char *key, size_t key_length,
                                   char *value, size_t *value_length, int offset, int lenth, const char *ip, int port);
    bool init_proxy(std::vector<std::string> ip_port_list);
    std::map<std::string, std::unique_ptr<proxy_proto::proxyService::Stub>>
    m_proxy_ptrs;
  };
}
#endif // CLIENT_H