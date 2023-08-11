#include "coordinator.h"


grpc::Status ECProject::CoordinatorImpl::get_key_info(
    ::grpc::ServerContext *context,
    const coordinator_proto::key_info *in_key_info,
    coordinator_proto::key_meta_info *return_proxy_ip_list
) {

    std::string key = in_key_info->key();
    int n = m_encoder->n_all_block_num();
    int k = m_encoder->k_data_block_num();

    for(int i = 0; i < k;i++){
        int node_id = m_stripe_info[key].node_ids[i];
        return_proxy_ip_list->add_ip(m_node_info[node_id].node_ip);
        return_proxy_ip_list->add_port(m_node_info[node_id].node_port);
    }
    
    return_proxy_ip_list->set_value_size_bytes(m_stripe_info[key].value_size_bytes);
    return grpc::Status::OK;
}