import argparse
def parse_args():
    parser = argparse.ArgumentParser(description='Parse the Experiment Parameters in the Command Line')
    
    parser.add_argument('--exp_type', type=str, default='encode', choices=['encode', 'repair', 'aggregate'],
                        required=True, help='Experiment type (default: "encode")')
    parser.add_argument('--dn_per_cluster_num', type=int, default=10, 
                        required=True, help='Number of datanoder per-cluster (default: 10)')
    parser.add_argument('--cluster_num', type=int, default=5, required=True, help='Number of clusters (default: 5)')
    
    parser.add_argument('--if_local', type=bool, default=True, required=True, help='If run the codes on one machine')
    group = parser.add_mutually_exclusive_group()#required=True
    group.add_argument('--sub_exp', type=str, default='all', choices=['diff_size', 'diff_bindwith',"all"],
                        help='Sub-experiment type (default: "diff_size")')
    # group.add_argument('--other_option', type=str, help='Another option related to sub_exp')
    args = parser.parse_args()
    
    # If exp_type is "encode", sub_exp must be provided.
    if args.exp_type == 'encode' and not args.sub_exp:
        parser.error('For "encode" experiment, --sub_exp must be provided.')

    return args
