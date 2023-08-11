#include "coordinator.h"


namespace ECProject
{

grpc::Status CoordinatorImpl::get_proxy_ip_ports(
    ::grpc::ServerContext *context,
    const coordinator_proto::ask_proxy_info *if_ask_proxy,
    coordinator_proto::proxy_ip_list *return_proxy_ip_list
){
    for (auto cur = m_cluster_info.begin(); cur != m_cluster_info.end(); cur++)
    {
        std::string proxy_ip_and_port = cur->second.proxy_ip + ":" + std::to_string(cur->second.proxy_port);
        return_proxy_ip_list->add_ip_port(proxy_ip_and_port);
    }    

    return grpc::Status::OK;
}

}