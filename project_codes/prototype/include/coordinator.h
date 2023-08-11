#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "meta_definition.h"

namespace ECProject
{

  class CoordinatorImpl final
      : public coordinator_proto::CoordinatorService::Service
  {
    public:
        CoordinatorImpl() {}

        grpc::Status setParameter(
            ::grpc::ServerContext *context,
            const coordinator_proto::Parameter *parameter,
            coordinator_proto::RepIfSetParaSucess *setParameterReply)override;

        grpc::Status set_key_coordinator(
            ::grpc::ServerContext *context,
            const coordinator_proto::SetRequest *key_value_size,
            coordinator_proto::ReplyIPPortList *ip_port_list) override;

        grpc::Status get_proxy_ip_ports(
          ::grpc::ServerContext *context,
          const coordinator_proto::ask_proxy_info *if_ask_proxy,
          coordinator_proto::proxy_ip_list *return_proxy_ip_list
        ) override;

        grpc::Status get_key_info(
          ::grpc::ServerContext *context,
          const coordinator_proto::key_info *in_key_info,
          coordinator_proto::key_meta_info *return_proxy_ip_list
        ) override;

        bool init_cluster_information(std::string cluster_information_path);
        bool init_proxy(std::string proxy_information_path);


    private:
        std::map<std::string, std::unique_ptr<proxy_proto::proxyService::Stub>>
        m_proxy_ptrs;
        ECSchema m_encode_parameter;
        std::map<unsigned int, ClusterItem> m_cluster_info;
        std::vector<int> m_cluster_ids;
        std::map<unsigned int, NodeItem> m_node_info;
        std::map<std::string, StripeItem> m_stripe_info;
        std::map<std::string, StripeItem> m_stripe_info_commiting;
        std::shared_ptr<Code_Placement> m_encoder;
        int random_select_one_node(int cluster_id);
  };


  class Coordinator
  {
  public:
    Coordinator(
        std::string m_coordinator_ip_port,
        std::string m_cluster_information_path)
        : m_coordinator_ip_port{m_coordinator_ip_port},
          m_cluster_information_path{m_cluster_information_path}
    {
      m_coordinatorImpl.init_cluster_information(m_cluster_information_path);
      m_coordinatorImpl.init_proxy(m_cluster_information_path);
    };
    // Coordinator
    void Run()
    {
      grpc::EnableDefaultHealthCheckService(true);
      grpc::reflection::InitProtoReflectionServerBuilderPlugin();
      grpc::ServerBuilder builder;
      std::string server_address(m_coordinator_ip_port);
      builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
      builder.RegisterService(&m_coordinatorImpl);
      std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
      std::cout << "Server listening on " << server_address << std::endl;
      server->Wait();
    }

  private:
    std::string m_coordinator_ip_port;
    std::string m_cluster_information_path;
    ECProject::CoordinatorImpl m_coordinatorImpl;
  };
}
#endif