
import xml.etree.ElementTree as ET
from exp_tools import config
cluster_information = {}
memcached_ip_port = {}
command_address = []
#address_command = [{"command_start":,"command_kill":,"command_check":,"check_number":,"ip":}]
def generate_cluster_info_dict(proxy_ip_list,cluster_ip_list,if_local=True,
                               datanode_number_per_cluster = 20,datanode_port_start=19000,
                               subdatanode_port_start=29000):
    gap_port_number = 100
    if if_local and datanode_number_per_cluster>100:
        raise ValueError("datanode_number_per_cluster is greater than gap_port_number")
    if len(proxy_ip_list)!=len(cluster_ip_list):
        raise ValueError("error len(proxy_ip_list)!=len(cluster_ip_list)")
    
    for i in range(len(cluster_ip_list)):
        for j in range(datanode_number_per_cluster):
            port = subdatanode_port_start + j
            if if_local:
                port = subdatanode_port_start + i*gap_port_number + j
            new_subnode_command = {}
            new_subnode_command["ip"] = cluster_ip_list[i][0]
            new_subnode_command["command_start"] = config.cmd_start_subdatnode(port)
            new_subnode_command["command_kill"] = 'pkill memcached'
            new_subnode_command["command_check"] = 'ps -aux | grep memcached | wc -l'
            command_address.append(new_subnode_command)
                
    proxy_num = len(proxy_ip_list)
                
    for i in range(proxy_num):
        new_cluster = {}
        new_cluster["proxy"] = proxy_ip_list[i][0]+":"+str(proxy_ip_list[i][1])

        new_proxy_command = {}
        new_proxy_command["ip"] = proxy_ip_list[i][0]
        new_proxy_command["command_start"] = config.cmd_start_proxy(proxy_ip_list[i][0],proxy_ip_list[i][1])
        new_proxy_command["command_kill"] = 'pkill run_proxy'
        new_proxy_command["command_check"] = 'ps -aux | grep run_proxy | wc -l'
                
        datanode_list = []
        for j in range(datanode_number_per_cluster):
            port = datanode_port_start + j
            subnode_port = subdatanode_port_start + j
            if if_local:
                port = datanode_port_start + i*gap_port_number + j
                subnode_port = subdatanode_port_start + i*gap_port_number+ j
            datanode_list.append([cluster_ip_list[i][0], port])
            
            new_datanode_command = {}
            new_datanode_command["ip"] = cluster_ip_list[i][0]
            new_datanode_command["command_start"] = config.cmd_start_datnode(ip=cluster_ip_list[i][0],port=port,
                                                                             sub_node_ip=cluster_ip_list[i][0],
                                                                             subnode_port=subnode_port)
            new_datanode_command["command_kill"] = 'pkill run_datanode'
            new_datanode_command["command_check"] = 'ps -aux | grep run_datanode | wc -l'
            command_address.append(new_datanode_command)
        command_address.append(new_proxy_command)
        new_cluster["datanode"] = datanode_list
        cluster_information[i] = new_cluster
    
    new_coordinator_command = {}
    new_coordinator_command["ip"] = config.COORDINATOR_IP
    new_coordinator_command["command_start"] = config.cmd_start_coordinator()
    new_coordinator_command["command_kill"] = 'pkill coordinator'
    new_coordinator_command["command_check"] = 'ps -aux | grep coordinator | wc -l'
    command_address.append(new_coordinator_command)
        
def generater_cluster_information_xml(file_name):
    root = ET.Element('clusters')
    root.text = "\n\t"
    for cluster_id in cluster_information.keys():
        cluster = ET.SubElement(root, 'cluster', {'id': str(cluster_id), 'proxy': cluster_information[cluster_id]["proxy"]})
        cluster.text = "\n\t\t"
        datanodes = ET.SubElement(cluster, 'datanodes')
        datanodes.text = "\n\t\t\t"
        for index,each_datanode in enumerate(cluster_information[cluster_id]["datanode"]):
            datanode = ET.SubElement(datanodes, 'datanode', {'uri': str(each_datanode[0])+":"+str(each_datanode[1])})
            #datanode.text = '\n\t\t\t'
            if index == len(cluster_information[cluster_id]["datanode"]) - 1:
                datanode.tail = '\n\t\t'
            else:
                datanode.tail = '\n\t\t\t'
        datanodes.tail = '\n\t'
        if cluster_id == len(cluster_information)-1:
            cluster.tail = '\n'
        else:
            cluster.tail = '\n\t'
    #root.tail = '\n'
    tree = ET.ElementTree(root)
    tree.write(file_name, encoding="utf-8", xml_declaration=True)

def generate_configuration_file(proxy_ip_list,cluster_ip_port,file_name,if_local=True,datanode_number_per_cluster = 20, 
                                datanode_port_start=config.DATANODE_PORT_START,
                                sub_datanode_port_start=config.SUBDATANODE_PORT_START):
    generate_cluster_info_dict(proxy_ip_list = proxy_ip_list,cluster_ip_list = cluster_ip_port,if_local=if_local,
                               datanode_number_per_cluster = datanode_number_per_cluster,datanode_port_start=datanode_port_start,
                               subdatanode_port_start=sub_datanode_port_start)
    generater_cluster_information_xml(file_name)
    
if __name__ == "__main__":
    proxy_ip_list = [
    ["10.0.0.11",50005],
    ["10.0.0.12",50005],
    ["10.0.0.13",50005],
    ]
    generate_cluster_info_dict(proxy_ip_list,proxy_ip_list,True)
    
    generater_cluster_information_xml("./cluster_infromation.xml")
    for item in command_address:
        print(item)
    # print(command_address)