from exp_tools.parse_parameters import parse_args

from exp_tools.generate_configs import generate_configuration_file,command_address
import exp_tools.config as config
from exp_tools.run_encode_exp import test_exp
from exp_tools.file_manage import deal_core_dump_files,clean_pyc_files
if __name__ == '__main__':
    args = parse_args()
    cluster_num = args.cluster_num
    dn_per_cluster_num = args.dn_per_cluster_num
    if_local = args.if_local
    
    if if_local:
        proxy_list = config.PROXY_IP_LIST_LOCAL[:cluster_num]
    else:
        proxy_list = config.PROXY_IP_LIST[:cluster_num]

    generate_configuration_file(proxy_ip_list = proxy_list,
                                cluster_ip_port = proxy_list,
                                file_name=config.CONFIG_FILE,
                                if_local=if_local,
                                datanode_number_per_cluster = dn_per_cluster_num, 
                                datanode_port_start=config.DATANODE_PORT_START,
                                sub_datanode_port_start=config.SUBDATANODE_PORT_START)
    if args.exp_type == "encode":
        if args.sub_exp=="all" or not args.sub_exp:
            test_exp(command_address)
    # for item in command_address:
    #     print(item)
    deal_core_dump_files()
    clean_pyc_files()
    
    