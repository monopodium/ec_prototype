#include "client.h"


bool ECProject::Client::set(std::string key, std::string value)
{
    grpc::ClientContext get_proxy_ip_port;
    coordinator_proto::SetRequest request;
    coordinator_proto::ReplyIPPortList reply;

    int n = m_encoder->n_all_block_num();
    int k = m_encoder->k_data_block_num();

    int block_size_byte = value_to_blocks_size(int(value.size()),k);

    int extend_value_size_byte = block_size_byte * k;

    request.set_key(key);
    request.set_valuesizebytes(value.size());

    grpc::Status status = m_coordinator_ptr->set_key_coordinator(
        &get_proxy_ip_port, request, &reply);

    for (int i = value.size(); i < extend_value_size_byte*2; i++)
    {
        value = value + '0';
    }

    if (!status.ok())
    {
      std::cout << "upload stripe failed!" << std::endl;
      return false;
    }
    else
    {

      std::vector<char *> v_data(k*2);
      std::vector<char *> v_coding(n - k);
      char **data = (char **)v_data.data();
      char **coding = (char **)v_coding.data();

      std::vector<std::vector<char>> v_area(k, std::vector<char>(block_size_byte));
      std::vector<std::vector<char>> v_coding_area(n - k, std::vector<char>(block_size_byte));

      for (int j = 0; j < k; j++)
      {
          std::copy(value.begin() + j * block_size_byte, value.begin() + (j+1) * block_size_byte, v_area[j].begin());
          data[j] = v_area[j].data();
      }
      for (int j = 0; j < n - k; j++)
      {
          coding[j] = v_coding_area[j].data();
      }
      m_encoder->encode(data, coding, block_size_byte);
      for (int i = 0; i<reply.block_trans_size() ;i++){
        if(i!=reply.block_trans(i).block_id()){
          std::cout<<"i:  "<<i<<std::endl;
          std::cout<<"reply.block_trans(i).block_id():  "<<reply.block_trans(i).block_id()<<std::endl;
          std::cout<<"order client set error!!!!!!!!!!!!!!!!"<<std::endl;
        }
        std::string key_id = key + std::to_string(reply.block_trans(i).block_id());
        std::string dest_datanode_ip = reply.block_trans(i).ip();
        int dest_datanode_port = reply.block_trans(i).port();

        if(reply.block_trans(i).if_direct()){
          if(i<k){
              set_to_datanode(key_id.c_str(), key_id.size(),
                                              data[i], block_size_byte, dest_datanode_ip.c_str(),dest_datanode_port);
          }else{
              set_to_datanode(key_id.c_str(), key_id.size(),
                                              coding[i-k], block_size_byte, dest_datanode_ip.c_str(),dest_datanode_port);
          }
        }else{
          
          grpc::ClientContext context;
          proxy_proto:: cal_block_inner_cluster inner_request;
          proxy_proto::if_ok inner_reply;
          for(int j = 0;j < reply.block_trans(i).from_blocks_size();j++){
            int from_each_block = reply.block_trans(i).from_blocks(j);
            inner_request.add_from_keys(key + std::to_string(from_each_block));
            inner_request.add_from_ips(reply.block_trans(from_each_block).ip());
            inner_request.add_from_ports(reply.block_trans(from_each_block).port());
          }
          std::cout<<"reply.block_trans(i).proxy_ip_port()"<<reply.block_trans(i).proxy_ip_port()<<std::endl;
          inner_request.set_dest_key(key_id);
          inner_request.set_dest_ip(dest_datanode_ip);
          inner_request.set_dest_port(dest_datanode_port);
          inner_request.set_block_size(block_size_byte);
          
          grpc::Status status = m_proxy_ptrs[reply.block_trans(i).proxy_ip_port()]->encode_inner(&context, inner_request, &inner_reply);
          if (!status.ok())
          {
            std::cout << "encode inner cluster failed!" << std::endl;
              return false;
          }else{
            if(!inner_reply.is_ok()){
              std::cout << "is not ok inner cluster failed!" << std::endl;
              return false;              
            }
          }
        }

        
      }     
    }

  return true;
}