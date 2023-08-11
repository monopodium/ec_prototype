#include "proxy.h"

int main(int argc, char **argv)
{
    if(argc != 3){
        std::cout << "./run_proxy coordinator_ip_port proxy_ip_port" << std::endl;
        std::cout << "./run_proxy 0.0.0.0:55555 0.0.0.0:15555" << std::endl;
        exit(-1); 
    }
    std::string coordinator_ip_port;
    std::string proxy_ip_port;
    coordinator_ip_port = std::string(argv[1]);
    proxy_ip_port = std::string(argv[2]);
    // config_path = std::string(argv[3]);

    std::cout<<"proxy_ip_port:      "<<proxy_ip_port<<std::endl;
    std::cout<<"coordinator_ip_port:"<<coordinator_ip_port<<std::endl;
    ECProject::Proxy proxy(proxy_ip_port, coordinator_ip_port);
    proxy.Run();
    return 0;
}