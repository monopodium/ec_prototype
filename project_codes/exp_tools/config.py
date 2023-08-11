COORDINATOR_IP="0.0.0.0"

COORDINATOR_PORT=10100
DATANODE_PORT_START=18000
SUBDATANODE_PORT_START=19000
LOCAL_HOST="0.0.0.0"#127.0.0.1
PROXY_IP_LOCAL=LOCAL_HOST
COORDINATOR_IP=LOCAL_HOST

CONFIG_FILE="/home/project/prototype/config/cluster_information.xml"
RESULT_PATH="/home/project/exp_result/result.txt"
IF_LOG=True
PROXY_IP_LIST_LOCAL = [
    [PROXY_IP_LOCAL,50001],
    [PROXY_IP_LOCAL,50002],
    [PROXY_IP_LOCAL,50003],
    [PROXY_IP_LOCAL,50004],
    [PROXY_IP_LOCAL,50005],
    [PROXY_IP_LOCAL,50006],
    [PROXY_IP_LOCAL,50007],
    [PROXY_IP_LOCAL,50008],
    [PROXY_IP_LOCAL,50009],
    [PROXY_IP_LOCAL,50010],
    [PROXY_IP_LOCAL,50011],
    [PROXY_IP_LOCAL,50012],
    [PROXY_IP_LOCAL,50013],
    [PROXY_IP_LOCAL,50014],
    [PROXY_IP_LOCAL,50015],
]
FORM_DIR= ""
DEST_DIR= ""
LIMITED_BANDWITH='9867100'
PROXY_IP_LIST = [
    [LOCAL_HOST,50001],
    [LOCAL_HOST,50002],
    [LOCAL_HOST,50003],
    [LOCAL_HOST,50004],
    [LOCAL_HOST,50005],
    [LOCAL_HOST,50006],
    [LOCAL_HOST,50007],
    [LOCAL_HOST,50008],
    [LOCAL_HOST,50009],
    [LOCAL_HOST,50010],
    [LOCAL_HOST,50011],
    [LOCAL_HOST,50012],
    [LOCAL_HOST,50013],
    [LOCAL_HOST,50014],
    [LOCAL_HOST,50015],
    [LOCAL_HOST,50016]
]
def cmd_start_client():
    return "./prototype/cmake/build/run_client"
def cmd_start_subdatnode(port,ip=LOCAL_HOST):
    return f"/home/memcached/bin/memcached -m 1024 -p {port} --max-item-size=16777216 -d -u root"

def cmd_start_datnode(sub_node_ip,subnode_port,port,ip=LOCAL_HOST,):
    if IF_LOG:
        return f"nohup ./prototype/cmake/build/run_datanode {ip}:{port} {sub_node_ip}:{subnode_port}> ./logs/run_datanode_{ip}_{port}.log 2>&1 &"
    else:
        return f"./prototype/cmake/build/run_datanode {ip}:{port} {sub_node_ip}:{subnode_port}"

def cmd_start_coordinator(port=COORDINATOR_PORT,config_file=CONFIG_FILE, ip=LOCAL_HOST):
    if IF_LOG:
        return f"nohup ./prototype/cmake/build/run_coordinator {ip}:{port} {config_file} > ./logs/run_coordinator_{port}.log 2>&1 &"
    else:
        return f"./prototype/cmake/build/run_coordinator {ip}:{port} {config_file}"

def cmd_start_proxy(proxy_ip,proxy_port,coordinator_ip=COORDINATOR_IP,coordinator_port=COORDINATOR_PORT):
    coordinator_address=coordinator_ip+":"+str(coordinator_port)
    proxy_address=proxy_ip+":"+str(proxy_port)
    if IF_LOG:
        return f"nohup ./prototype/cmake/build/run_proxy {coordinator_address} {proxy_address} > ./logs/run_proxy_{proxy_ip}_{proxy_port}.log 2>&1 &"
    else:
        return f" ./prototype/cmake/build/run_proxy {coordinator_address} {proxy_address}"

def cmd_ssh_move_dir(start_dir,ip,end_dir):
    if ip==LOCAL_HOST:
        return ""
    else:
        return f"rsync -rtvpl {start_dir} msms@{ip}:{end_dir}"
def cmd_ssh_run(ip,command):
    if ip==LOCAL_HOST:
        return command
    else:
        return f"ssh msms@{ip} '{command}'"

def cmd_wondershaper_init(ip,if_local=True):
    if if_local:
        return 'git clone https://github.com/magnific0/wondershaper.git'
    else: 
        return f"ssh msms@{ip} 'git clone https://github.com/magnific0/wondershaper.git'"

def cmd_wondershaper_close(ip,NIC,if_local=True):
    if if_local:
        return f'sudo ./wondershaper/wondershaper -c -a {NIC}'
    else:
        return f"ssh msms@{ip} 'sudo ./wondershaper/wondershaper -c -a {NIC}'"
def cmd_wondershaper_start(ip,NIC,bandwidth,if_local=True):
    if if_local:
        return f'sudo ./wondershaper/wondershaper -a {NIC} -d {bandwidth}'
    else:
        return f"ssh msms@{ip} 'sudo ./wondershaper/wondershaper -a {NIC} -d {bandwidth}'"

def compile():
    return "cd ./prototype/cmake/build;cmake ../..;make -j"


placement_option_default = ["Flat", "Random", "Best_Placement","Sub_Placement"]
parameter_option_default = [(7, 4, 2), (10, 6, 3), (12, 8, 4), (15, 10, 5), (16, 10, 5)]
data_size_option_default = [1, 4, 16, 256, 1024, 4096]
bindwith_option_default = [493355, 657806, 986710, 1973420, 9867100]
encode_transfer_type = ["Basic","Optimized"]


default_exp_parameter = {
    'transfer_type':"Optimized",
    'encode_type':"Azure_LRC",
    'placement_type':"Sub_Optimal",
    'parameter': "8 2 2",#(n,k,r) k=8,l=2,r=4,g=4
    'trace_type':"Random",
    'value_length':1024,
    'client_address':f'{LOCAL_HOST}:44444', 
    'coordinator_address':f'{LOCAL_HOST}:{COORDINATOR_PORT}'
}

data_all_xorbas = {
    'code_type':"Xorbas",
    'placement_option' : placement_option_default,
    'parameter_option': parameter_option_default,
    'data_size_option':data_size_option_default,
    'bindwith_option':bindwith_option_default,
    "placement": "Random",
    "parameter": (12, 8, 4), 
    "data_size": 1024, 
    "bindwith": 986710, 
    "DRC": 1, 
    "NRR": 1, 
    "maxDRC": 1, 
    "minDRC": 1, 
    "maxNRR": 1, 
    "minNRR": 1
}

data_all_azure_lrc = {
    'code_type':"Azure_LRC",
    'placement_option' : ["Flat", "Random", "Best_Placement","Sub_Optimal"],
    #'parameter_option': [(12,6,3),(16,10,5),(16,12,6),(18,12,5),(24,15,5)],
    'parameter_option': [(8, 3, 3), (9, 4, 2), (12, 6, 3), (16,12,6), (20, 12, 3), (24, 15, 5)],
    'data_size_option':data_size_option_default,
    'bindwith_option':bindwith_option_default,
    "placement": "Random",
    "parameter": (16,12,6), 
    "data_size": 1024, 
    "bindwith": 986710, 
    "DRC": 1, 
    "NRR": 1, 
    "maxDRC": 1, 
    "minDRC": 1, 
    "maxNRR": 1, 
    "minNRR": 1
}
data_all_azure_lrc_1 = {
    'code_type':"Azure_LRC_1",
    'placement_option' : placement_option_default,
    'parameter_option': [(12,6,3),(15,10,5),(17,12,6),(18,12,5),(24,15,5)],
    'data_size_option':data_size_option_default,
    'bindwith_option':bindwith_option_default,
    "placement": "Random",
    "parameter": (17,12,6), 
    "data_size": 1024, 
    "bindwith": 986710, 
    "DRC": 1, 
    "NRR": 1, 
    "maxDRC": 1, 
    "minDRC": 1, 
    "maxNRR": 1, 
    "minNRR": 1
}
data_all_optimal = {
    'code_type':"Optimal_LRC",
    'placement_option' : placement_option_default,
    'parameter_option': parameter_option_default,
    'data_size_option':data_size_option_default,
    'bindwith_option':bindwith_option_default,
    "placement": "Random",
    "parameter": (12, 8, 4), 
    "data_size": 1024, 
    "bindwith": 986710, 
    "DRC": 1, 
    "NRR": 1, 
    "maxDRC": 1, 
    "minDRC": 1, 
    "maxNRR": 1, 
    "minNRR": 1
}
data_all_optimal["parameter"]

default_data = {
    "Xorbas":data_all_xorbas,
    "Azure_LRC":data_all_azure_lrc,
    "Azure_LRC_1":data_all_azure_lrc_1,
    "Optimal_LRC":data_all_optimal
}
