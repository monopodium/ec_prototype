#include "coordinator.h"

int main(int argc, char **argv)
{
  if(argc != 3){
      std::cout << "./run_coordinator ip_port config_path" << std::endl;
      std::cout << "./run_coordinator 0.0.0.0:55555 /home/project/prototype/config/AZInformation.xml" << std::endl;
      exit(-1); 
  }
  std::string coordinator_ip_port = "0.0.0.0:55555";
  std::string config_path = "/home/project/prototype/config/AZInformation.xml";
  coordinator_ip_port = std::string(argv[1]);
  config_path = std::string(argv[2]);
  std::cout << "Current working directory: " << config_path << std::endl;
  ECProject::Coordinator coordinator(coordinator_ip_port, config_path);
  coordinator.Run();
  return 0;
}