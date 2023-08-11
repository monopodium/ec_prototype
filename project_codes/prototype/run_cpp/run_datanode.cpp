#include "datanode.h"

int main(int argc, char **argv)
{
    std::string ip_and_port(argv[1]);
    std::string ip = ip_and_port.substr(0, ip_and_port.find(":"));
    int port = std::stoi(ip_and_port.substr(ip_and_port.find(":") + 1, ip_and_port.size()));

    std::string sub_ip_and_port(argv[2]);
    std::string sub_ip = sub_ip_and_port.substr(0, sub_ip_and_port.find(":"));
    int sub_port = std::stoi(sub_ip_and_port.substr(sub_ip_and_port.find(":") + 1, sub_ip_and_port.size()));

    ECProject::DataNode datanode(ip,port,sub_ip, sub_port);
    datanode.start();
    return 0;
}