#include "client.h"
#include "toolbox.h"
#include <fstream>

int main(int argc, char **argv)
{
  if(argc != 11){
      std::cout << "./run_client transfer_type encode_type placement_type k real_l g trace_type value_length client_address coordinator_address" << std::endl;
      std::cout << "./run_client Basic RS Sub_Optimal 3 -1 2 random 2048 0.0.0.0:44444 0.0.0.0:55555" << std::endl;
      exit(-1); 
  }
  std::string client_address,client_ip;
  int client_port;
  int value_length;

  value_length = std::stoi(std::string(argv[8]));
  client_address = std::string(argv[9]);
  client_ip = client_address.substr(0,client_address.find(':'));
  client_port = std::stoi(client_address.substr(client_address.find(':')+1,client_address.size()));
  std::cout<<"   client_ip: "<<client_ip;
  std::cout<<"   client_port: "<<client_port<<std::endl;
  ECProject::Client client(client_ip, client_port);
  client.parse_command(argc, argv);
  std::string key = "1234567812345678";
  std::string value = "12345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678";
  client.set(key,value);

  std::unordered_map<std::string, std::string> key_values;
  std::cout<<client.get(key)<<std::endl;;
  if(client.get(key)!=value){
    std::cout<<"set error!!!!!!!!"<<std::endl;
  }
}