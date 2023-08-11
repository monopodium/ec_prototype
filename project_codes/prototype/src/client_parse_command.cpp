#include "client.h"

bool ECProject::Client::parse_command(int argc, char **argv){
    bool partial_decoding = true;
    int k, real_l, g_m, b;
    
    ECProject::EncodeTransferType transfer_type;
    ECProject::EncodeType encode_type;
    ECProject::PlacementType placement_type;
    
    std::string coordinator_address;
    std::string coordinator_ip;
    int coordinator_port;
    if(std::string(argv[1])=="Basic"){
        transfer_type = ECProject::Basic;
    }else if(std::string(argv[1])=="Optimized"){
        transfer_type = ECProject::Optimized;
    }
    else{
        std::cout << "error: unknown transfer_type" << std::endl;
        exit(-1);
    }
    if(std::string(argv[2])=="Xorbas")
    {
        encode_type = ECProject::Xorbas;
    }
    else if(std::string(argv[2])=="Azure_LRC")
    {
        encode_type = ECProject::Azure_LRC;
    }
    else if(std::string(argv[2])=="Azure_LRC_1")
    {
        encode_type = ECProject::Azure_LRC_1;
    }
    else if(std::string(argv[2])=="Optimal_LRC")
    {
        encode_type = ECProject::Optimal_LRC;
    }else{
        std::cout << "error: unknown encode_type" << std::endl;
        exit(-1);
    }

    if(std::string(argv[3])=="Flat"){
        placement_type = ECProject::Flat;
    }else if(std::string(argv[3])=="Random"){
        placement_type = ECProject::Random;
    }else if(std::string(argv[3])=="Best_Placement"){
        placement_type = ECProject::Best_Placement;
    }else if(std::string(argv[3])=="Sub_Optimal"){
        placement_type = ECProject::Sub_Optimal;
    }else{
        std::cout << "error: unknown placement_type" << std::endl;
        exit(-1);
    }

    k = std::stoi(std::string(argv[4]));
    real_l = std::stoi(std::string(argv[5]));
    g_m = std::stoi(std::string(argv[6]));
    
    
    coordinator_address = std::string(argv[10]);
    coordinator_ip = coordinator_address.substr(0,coordinator_address.find(':'));
    coordinator_port = std::stoi(coordinator_address.substr(coordinator_address.find(':')+1,coordinator_address.size()));
    set_coordinator(coordinator_address);


    ECSchema temp(partial_decoding, encode_type, placement_type,transfer_type, k, real_l, g_m);
    m_encode_parameter = temp;

    if (m_encode_parameter.encodetype == Xorbas)
    {
        m_encoder = std::make_shared<ECProject::Xorbas_Class>();
    }
    else if (m_encode_parameter.encodetype == Optimal_LRC)
    {
        m_encoder = std::make_shared<ECProject::Optimal_LRC_Class>();
    }
    else if (m_encode_parameter.encodetype == Azure_LRC_1)
    {
        m_encoder = std::make_shared<ECProject::Azure_LRC_1_Class>();
    }
    else
    {
        m_encoder = std::make_shared<ECProject::Azure_LRC_Class>();
    }
    m_encoder->set_parameter(-1,m_encode_parameter.k_datablock,-1,
    m_encode_parameter.g_m_globalparityblock,m_encode_parameter.l_localgroup,8);
    
    if (set_parameter_grpc({partial_decoding, encode_type, placement_type, transfer_type, k, real_l, g_m}))
    {
        std::cout << "set parameter successfully!" << std::endl;
        return true;
    }else
    {
        std::cout << "Failed to set parameter!" << std::endl;
        return false;
    }

}