from exp_tools.start_close_nodes import start_close_nodes,traffic_control_onenode,\
                                        start_client,clean_logs,compile
import exp_tools.config as config
from exp_tools.generate_configs import command_address
record_exp = set()
def exp_base(diff_type, diff_list,command_address,limit_ip_list=[]):
    default_parameter = config.default_exp_parameter
    for item in diff_list:
        cmd = config.cmd_start_client()
        for each_para in default_parameter.keys():
            if each_para == diff_type:
                cmd = cmd + " " + str(item)
            else:
                cmd = cmd + " " + str(default_parameter[each_para])
        with open(config.RESULT_PATH, 'a') as file:
            file.write(cmd+'\n')
        clean_logs()
        compile()
        traffic_control_onenode(action="limit",ip_list=limit_ip_list,bandwidth=config.LIMITED_BANDWITH)
        start_close_nodes('start',command_address,config.FORM_DIR,config.DEST_DIR)
        
        start_client(cmd)
        start_close_nodes('close',command_address,config.FORM_DIR,config.DEST_DIR)
        traffic_control_onenode(action="lift_limit",ip_list=limit_ip_list,bandwidth=config.LIMITED_BANDWITH)
        

def test_exp(command_address):
    exp_base(diff_type="test", diff_list=[1],
             command_address=command_address)