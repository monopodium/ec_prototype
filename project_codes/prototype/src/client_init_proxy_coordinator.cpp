#include "client.h"

bool ECProject::Client::init_proxy(std::vector<std::string> ip_port_list)
{

    for(auto proxy_ip_port:ip_port_list){
        auto _stub = proxy_proto::proxyService::NewStub(grpc::CreateChannel(proxy_ip_port, grpc::InsecureChannelCredentials()));
        proxy_proto::CheckaliveCMD Cmd;
        proxy_proto::RequestResult result;
        grpc::ClientContext clientContext;
        Cmd.set_name("test");
        grpc::Status status;
        
        status = _stub->checkalive(&clientContext, Cmd, &result);
        if (status.ok())
        {
        }else{
            std::cout<<"proxy_ip_port"<<proxy_ip_port<<std::endl;
            std::cout << "checkalive,fail" << std::endl;
        }

        m_proxy_ptrs.insert(std::make_pair(proxy_ip_port, std::move(_stub)));
    }
    return true;
}
bool ECProject::Client::set_coordinator(std::string coordinator_ip_port){
    m_coordinator_ip_port = coordinator_ip_port;
    auto channel = grpc::CreateChannel(m_coordinator_ip_port, grpc::InsecureChannelCredentials());
    m_coordinator_ptr = coordinator_proto::CoordinatorService::NewStub(channel);
    
    coordinator_proto::ask_proxy_info Cmd;
    coordinator_proto::proxy_ip_list result;
    grpc::ClientContext clientContext;
    Cmd.set_ask_proxy(true);
    m_coordinator_ptr->get_proxy_ip_ports(&clientContext, Cmd, &result);
    std::vector<std::string> proxy_list;
    for(int i = 0;i < result.ip_port_size();i++){
        proxy_list.push_back(result.ip_port(i));
    }
    init_proxy(proxy_list);
}