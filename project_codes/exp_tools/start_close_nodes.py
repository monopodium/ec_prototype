import subprocess
import exp_tools.config as config

def clean_logs():
    subprocess.run("rm -rf ./logs/*", shell=True)

def compile():
    subprocess.run(config.compile(), shell=True)
    
def start_client(command):
    print(command)
    subprocess.run(command, shell=True)
#address_command = [{"command_start":,"command_kill":,"command_check":,"check_number":,"ip":}]
def start_close_nodes(action,address_command,start_dir=None,end_dir=None):
    if action=="init":
        if not start_dir:
            raise ValueError("start_dir is None")
        if not end_dir:
            raise ValueError("end_dir is None")
        for nodes in address_command:
            subprocess.run(config.cmd_ssh_move_dir(start_dir,nodes['ip'],end_dir), shell=True)
    if action=="start":
        for nodes in address_command:
            # print(nodes)
            #subprocess.run(config.cmd_ssh_run(nodes['ip'],nodes['command_kill']), shell=True)
            subprocess.run(config.cmd_ssh_run(nodes['ip'],nodes['command_start']), shell=True, stdout=subprocess.PIPE, text=True) 
    if action=="close":
        for nodes in address_command:
            subprocess.run(config.cmd_ssh_run(nodes['ip'],nodes['command_kill']), shell=True)
            
    print("state after "+ action)
    ip_command_set = set()
    for nodes in address_command:
        if nodes["ip"]+nodes['command_check'] not in ip_command_set:
            print(nodes["ip"], nodes['command_check'],"==",
                  subprocess.run(config.cmd_ssh_run(nodes['ip'],nodes['command_check']), stdout=subprocess.PIPE, text=True, shell=True).stdout)
            ip_command_set.add(nodes["ip"]+nodes['command_check'])

def traffic_control_onenode(ip_list,bandwidth,action="limit",NIC='enp4s0f1'):
    for ip_port in ip_list:
        ip = ip_port[0]
        if action=="init":
            subprocess.run(config.cmd_wondershaper_init(ip), shell=True)
        if action=="limit":
            subprocess.run(config.cmd_wondershaper_close(ip,NIC), shell=True)
            subprocess.run(config.cmd_wondershaper_start(ip,NIC,bandwidth), shell=True)      
        if action=="lift_limit":
            subprocess.run(config.cmd_wondershaper_close(ip,NIC), shell=True)
        