#include "proxy.h"
grpc::Status ECProject::ProxyImpl::encode_inner(grpc::ServerContext *context,
                        const proxy_proto::cal_block_inner_cluster *request,
                        proxy_proto::if_ok *response)
{
    std::vector<std::string> from_keys;
    std::vector<std::string> form_ips;
    std::vector<int> form_ports;

    for (int i = 0; i < request->from_keys_size(); i++)
    {
      std::cout<<"from_keys:"<<request->from_keys(i)<<std::endl;
      from_keys.push_back(request->from_keys(i));
    }
    for (int i = 0; i < request->from_ips_size(); i++)
    {
      std::cout<<"from_keys:"<<request->from_ips(i)<<std::endl;
      form_ips.push_back(request->from_ips(i));
    }
    for (int i = 0; i < request->from_ports_size(); i++)
    {
      std::cout<<"from_keys:"<<request->from_ports(i)<<std::endl;
      form_ports.push_back(request->from_ports(i));
    }

    std::string dest_key=request->dest_key();
    std::string dest_ip=request->dest_ip();
    int dest_port=request->dest_port();
    int block_size_bytes=request->block_size();

    auto block_ptr = std::make_shared<std::vector<std::vector<char>>>();
    int r_blocks = from_keys.size();
    std::vector<char *> v_data(r_blocks);
    std::vector<char *> v_coding(1);
    char **data = v_data.data();
    char **coding = v_coding.data();
    auto myLock_ptr = std::make_shared<std::mutex>();

    auto get_from_node = [this, block_ptr,block_size_bytes,myLock_ptr]
    (std::string key, std::string ip, int port)
    {
        std::vector<char> temp(block_size_bytes);
        get_from_datanode(key.c_str(), key.size(),
                                  temp.data(), (size_t*)block_size_bytes, 0, block_size_bytes, ip.c_str(), port);
        myLock_ptr->lock();
        block_ptr->push_back(temp);
        myLock_ptr->unlock();
    };

    std::vector<std::vector<char>> v_data_area(r_blocks, std::vector<char>(block_size_bytes));
    std::vector<std::vector<char>> v_coding_area(1, std::vector<char>(block_size_bytes));
    for (int j = 0; j < r_blocks; j++)
    {
      data[j] = v_data_area[j].data();
    }
    coding[0] = v_coding_area[0].data();

    std::vector<std::thread> read_from_datanodes;
    for (int j = 0; j < r_blocks; j++)
    {
        read_from_datanodes.push_back(std::thread(get_from_node,from_keys[j],form_ips[j],form_ports[j]));
    }
    for (int j = 0; j < r_blocks; j++)
    {
      read_from_datanodes[j].join();
    }

    for(int j = 0; j < r_blocks; j++){
        memcpy(data[j], (*block_ptr)[j].data(), block_size_bytes);
    }
    encode_one(data, coding, r_blocks, block_size_bytes);
    if(set_to_datanode(dest_key.c_str(), dest_key.size(),
                                coding[0], block_size_bytes,dest_ip.c_str(), dest_port)){
        response->set_is_ok(false);
      }{
        response->set_is_ok(true);
      }
    return grpc::Status::OK;
}