#include "client.h"



std::string ECProject::Client::get(std::string key)
{
    coordinator_proto::key_info Cmd;
    coordinator_proto::key_meta_info result;
    grpc::ClientContext clientContext;
    Cmd.set_key(key);
    m_coordinator_ptr->get_key_info(&clientContext, Cmd, &result);
    std::vector<std::string> proxy_list;
    int k = result.ip_size();

    int value_size = result.value_size_bytes();
    int block_size_bytes = value_to_blocks_size(value_size,k);

    auto block_ptr = std::make_shared<std::vector<std::vector<char>>>();
    std::vector<char *> v_data(k);
    char **data = v_data.data();
    auto myLock_ptr = std::make_shared<std::mutex>();
    std::vector<std::vector<char>> v_data_area(k, std::vector<char>(block_size_bytes));
    for (int j = 0; j < k; j++)
    {
      data[j] = v_data_area[j].data();
    }

    std::string return_str;
    return_str.resize(value_size);





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

    std::vector<std::thread> read_from_datanodes;

    for (int j = 0; j < k; j++)
    {
        std::string key_part = key+std::to_string(j);
        read_from_datanodes.push_back(std::thread(get_from_node,key_part,
        result.ip(j),result.port(j)));
    }
    for (int j = 0; j < k; j++)
    {
      read_from_datanodes[j].join();
    }
    for(int j = 0; j < k; j++){
        int exact_size = std::min(value_size-block_size_bytes*j,block_size_bytes);
        memcpy((char*)(&return_str[j*block_size_bytes]), (*block_ptr)[j].data(), exact_size);
    }
    return return_str;
}