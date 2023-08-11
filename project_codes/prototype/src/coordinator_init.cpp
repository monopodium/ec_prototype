#include "coordinator.h"


bool ECProject::CoordinatorImpl::init_proxy(std::string proxy_information_path)
{
    for (auto cur = m_cluster_info.begin(); cur != m_cluster_info.end(); cur++)
    {
        std::string proxy_ip_and_port = cur->second.proxy_ip + ":" + std::to_string(cur->second.proxy_port);
        // std::string proxy_ip_and_port = "172.20.0.2:" + std::to_string(cur->second.proxy_port);
        auto _stub = proxy_proto::proxyService::NewStub(grpc::CreateChannel(proxy_ip_and_port, grpc::InsecureChannelCredentials()));
        proxy_proto::CheckaliveCMD Cmd;
        proxy_proto::RequestResult result;
        grpc::ClientContext clientContext;
        Cmd.set_name("test");
        grpc::Status status;
        std::cout<<"proxy_ip_and_port"<<proxy_ip_and_port<<std::endl;
        status = _stub->checkalive(&clientContext, Cmd, &result);
        if (status.ok())
        {
        std::cout << "checkalive,ok" << std::endl;
        }else{
        std::cout << "checkalive,fail" << std::endl;
        }
        m_proxy_ptrs.insert(std::make_pair(proxy_ip_and_port, std::move(_stub)));
    }
    return true;
}
bool ECProject::CoordinatorImpl::init_cluster_information(std::string cluster_information_path)
{
    std::cout << "cluster_information_path:" << cluster_information_path << std::endl;
    tinyxml2::XMLDocument xml;
    xml.LoadFile(cluster_information_path.c_str());
    tinyxml2::XMLElement *root = xml.RootElement();
    int node_id = 0;
    for (tinyxml2::XMLElement *cluster = root->FirstChildElement(); cluster != nullptr; cluster = cluster->NextSiblingElement())
    {
        std::string cluster_id(cluster->Attribute("id"));
        std::string proxy(cluster->Attribute("proxy"));
        std::cout << "cluster_id: " << cluster_id << " , proxy: " << proxy << std::endl;
        m_cluster_info[std::stoi(cluster_id)].cluster_id = std::stoi(cluster_id);
        m_cluster_ids.push_back(std::stoi(cluster_id));
        auto pos = proxy.find(':');
        m_cluster_info[std::stoi(cluster_id)].proxy_ip = proxy.substr(0, pos);
        m_cluster_info[std::stoi(cluster_id)].proxy_port = std::stoi(proxy.substr(pos + 1, proxy.size()));
        for (tinyxml2::XMLElement *node = cluster->FirstChildElement()->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
        {
            std::string node_uri(node->Attribute("uri"));
            std::cout << "node: " << node_uri << std::endl;
            m_cluster_info[std::stoi(cluster_id)].node_ids.push_back(node_id);
            m_node_info[node_id].node_id = node_id;
            auto pos = node_uri.find(':');
            m_node_info[node_id].node_ip = node_uri.substr(0, pos);
            m_node_info[node_id].node_port = std::stoi(node_uri.substr(pos + 1, node_uri.size()));
            m_node_info[node_id].cluster_id = std::stoi(cluster_id);
            node_id++;
        }
    }
    return true;
}