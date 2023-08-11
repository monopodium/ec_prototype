#ifndef PROXY_H
#define PROXY_H
#include "meta_definition.h"


namespace ECProject
{

class ProxyImpl final
: public proxy_proto::proxyService::Service,
public std::enable_shared_from_this<ECProject::ProxyImpl>
{
    public:
        ProxyImpl(std::string proxy_ip_port, std::string coordinator_ip_port) :
        m_proxy_ip_socket(proxy_ip_port.substr(0,proxy_ip_port.find(':')).c_str())
        ,m_proxy_port_socket(std::stoi(proxy_ip_port.substr(proxy_ip_port.find(':') + 1, proxy_ip_port.size()))+10000)
        ,m_coordinator_ip_port(coordinator_ip_port)
        ,m_proxy_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::address::from_string(m_proxy_ip_socket),m_proxy_port_socket))
        {
        
            init_coordinator();
        };
        ~ProxyImpl() {  };
        grpc::Status checkalive(grpc::ServerContext *context,
                                const proxy_proto::CheckaliveCMD *request,
                                proxy_proto::RequestResult *response) override;
        grpc::Status encode_inner(grpc::ServerContext *context,
                                const proxy_proto::cal_block_inner_cluster *request,
                                proxy_proto::if_ok *response) override;
    private:
        bool init_coordinator();
        std::unique_ptr<coordinator_proto::CoordinatorService::Stub>
            m_coordinator_stub;
        // std::string m_config_path;
        std::string m_proxy_ip_socket;
        int m_proxy_port_socket;
        std::string m_coordinator_ip_port;
        asio::io_context m_io_context;
        asio::ip::tcp::acceptor m_proxy_acceptor;
        bool set_to_datanode(const char *key, size_t key_length,
                                 const char *value, size_t value_length, const char *ip, int port);
        bool get_from_datanode(const char *key, size_t key_length,
                                   char *value, size_t *value_length, int offset, int lenth, const char *ip, int port);

};
class Proxy
{
public:
    Proxy(std::string proxy_ip_port, std::string coordinator_ip_port) : m_proxy_ip_port(proxy_ip_port), m_proxyImpl_ptr(proxy_ip_port, coordinator_ip_port) {}
    void Run()
    {
        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        grpc::ServerBuilder builder;
        std::cout << "proxy_ip_port:" << m_proxy_ip_port << std::endl;
        builder.AddListeningPort(m_proxy_ip_port, grpc::InsecureServerCredentials());
        builder.RegisterService(&m_proxyImpl_ptr);
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        server->Wait();
    }
    

private:
    // std::shared_ptr<OppoProject::ProxyImpl> m_proxyImpl_ptr;
    std::string m_proxy_ip_port;
    ECProject::ProxyImpl m_proxyImpl_ptr;
};
    
}
#endif