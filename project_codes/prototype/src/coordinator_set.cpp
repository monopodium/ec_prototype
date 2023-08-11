#include "coordinator.h"

int ECProject::CoordinatorImpl::random_select_one_node(int cluster_id){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> urd(0, m_cluster_info[cluster_id].node_ids.size() - 1);
    int random_number = urd(gen);
    // std::cout<<"random_number:"<<random_number<<std::endl;
    int node_id = m_cluster_info[cluster_id].node_ids[random_number];
    return node_id;
}
grpc::Status ECProject::CoordinatorImpl::set_key_coordinator(
    ::grpc::ServerContext *context,
    const coordinator_proto::SetRequest *key_value_size,
    coordinator_proto::ReplyIPPortList *ip_port_list){
        std::cout<<"test"<<std::endl;
    try {
        std::string key = key_value_size->key();
        int value_size_byte = key_value_size->valuesizebytes();
        TransferPlan transferplan;
        m_encoder->encode_tansfer_plan(m_encode_parameter.encode_transfer_type,transferplan,m_encode_parameter.placementtype);   
        m_encoder->print_placement_raw(m_encode_parameter.placementtype);
        std::cout<<"transferplan"<<transferplan.size()<<std::endl;
        m_encoder->print_transferplan(transferplan); 



        std::vector<int> cluster_ids_dis(m_cluster_ids);
        std::random_shuffle(cluster_ids_dis.begin(), cluster_ids_dis.end(), MyRand());
        std::vector<int> all_node_ids;
        StripeItem stripe_this;
        stripe_this.value_size_bytes = value_size_byte;

        for(int i = 0;i < transferplan.size();i++){
            coordinator_proto::ReplyIPPortTrans trans_block_grpc;
            auto block_trans = transferplan[i];
            trans_block_grpc.set_block_id(i);
            int dis_cluster;
            if(block_trans[0]==-1){
                dis_cluster = cluster_ids_dis[block_trans[1]%cluster_ids_dis.size()];

                trans_block_grpc.set_if_direct(true);
            }else{
                trans_block_grpc.set_if_direct(false);
                for(int j = 0;j<block_trans.size();j++){
                    if(block_trans[j]==-1){
                        dis_cluster = cluster_ids_dis[block_trans[j+1]%cluster_ids_dis.size()];
                        break;
                    }else{
                        trans_block_grpc.add_from_blocks(block_trans[j]);
                    }
                }                
                
            }
            int dis_node_id = random_select_one_node(dis_cluster);
            while (std::find(all_node_ids.begin(), all_node_ids.end(), dis_cluster * 100 + dis_node_id) != all_node_ids.end())
            {
                dis_node_id = random_select_one_node(dis_cluster);
                
            }
            all_node_ids.push_back(dis_cluster * 100 + dis_node_id);

            trans_block_grpc.set_port(m_node_info[dis_node_id].node_port);
            trans_block_grpc.set_ip(m_node_info[dis_node_id].node_ip);
            trans_block_grpc.set_proxy_ip_port(m_cluster_info[dis_cluster].proxy_ip+":"+std::to_string(m_cluster_info[dis_cluster].proxy_port));

            ip_port_list->add_block_trans()->CopyFrom(trans_block_grpc);

            stripe_this.node_ids.push_back(dis_node_id);
        }

        m_stripe_info[key] = stripe_this;    
    }
    catch(std::exception &e){
        std::cout << "exception" << std::endl;
        std::cout << e.what() << std::endl;
    }

    return grpc::Status::OK;
    }
