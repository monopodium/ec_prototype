#ifndef UTILS_H
#define UTILS_H

#include "lrc_definition.h"

namespace ECProject
{
    bool encode_one(char **data_ptrs, char **coding_ptrs, int block_num,int blocksize);
    class Code_Placement
    {
        // 类型有三：数据块，局部校验块，全局校验块
        // [[data_block1,data_block2...][],[global_parity1,global_parity1..]]
        // 数据块映射到组别
        // {"data_block":{"group:number":,"repair_request":}}
    public:
        Code_Placement(int n_in = 16, int k_in = 10, int r_in = 5, int w = 8){
            m_n = n_in;
            m_k = k_in;
            m_r = r_in;
            // this->Code_Placement::set_parameter(n_in, k_in, r_in);
        };
        virtual ~Code_Placement(){};
        virtual void set_debug(bool if_debug);
        virtual void print_information();
        void set_parameter(int n_in = -1, int k_in= -1, int r_in= -1,int g_in= -1,int l_in= -1, int w = 8);
        virtual std::pair<double, double> return_DRC_NRC(ECProject::PlacementType placement_type,int seed = 10);
        virtual ECProject::Placement generate_placement(ECProject::PlacementType placement_type, int random_seed = 10);
        // virtual void generate_best_placement() = 0;
        virtual int calculate_distance() = 0;
        virtual void nkr_to_klgr(int n, int k, int r) = 0;
        virtual void klgr_to_nkr(int k, int l, int g, int r) = 0;
        virtual bool check_parameter() = 0;
        virtual bool encode(char **data_ptrs, char **coding_ptrs, int blocksize)=0;
        virtual bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize)=0;
        virtual int block_to_index(std::string block);
        
        virtual void print_placement_raw(PlacementType placement_type);
        virtual void repair_request(int index,std::vector<int> &vec);
        virtual int k_data_block_num();
        virtual int g_global_block_num();
        virtual int l_local_block_num();
        virtual int n_all_block_num();
        virtual int r_group_block_num();
        virtual bool decode_in_group_xor(int group_data_number, char **data_ptrs, char **coding_ptrs, int blocksize);
        std::string s_index_to_string(int index);
        void print_transferplan(const TransferPlan transferplan);
        virtual bool encode_tansfer_plan(EncodeTransferType encode_transfer_type, TransferPlan& transfer_plan, PlacementType placement_type = PlacementType::Sub_Optimal);
        virtual std::pair<double, double> encode_tansfer_cost_balance(EncodeTransferType encode_transfer_type, PlacementType placement_type = PlacementType::Sub_Optimal);

    protected:
        int m_n = 0;
        int m_k = 0;
        int m_r = 0;
        int m_l = 0;
        int m_g = 0;
        bool m_l_stable = false;

        int m_d = 0;
        int m_w = 8;
        bool m_if_debug = false;
        bool m_first = true;
        std::vector<std::string> m_raw_stripe;
        std::vector<std::vector<std::string>> m_stripe_information;
        RepairRequest m_block_repair_request;

        
        PlacementRaw m_random_placement_raw;
        std::map<std::string, int> m_random_placement_map;

        PlacementRaw m_flat_placement_raw;
        std::map<std::string, int> m_flat_placement_map;

        PlacementRaw m_best_placement_raw;
        std::map<std::string, int> m_best_placement_map;

        PlacementRaw m_sub_optimal_placement_raw;
        std::map<std::string, int> m_sub_optimal_placement_map;

        std::map<std::string, int> m_block_repair_cost;
        std::map<std::string, int> m_block_to_groupnumber;

        double return_NRC();
        double return_DRC();
        const PlacementRaw select_placement_raw(PlacementType placement_type);
        void generate_repair_cost(std::map<std::string, int> m_placement_map);
        std::string index_to_str(std::string block_type, int index);
        void generate_raw_information();
        virtual void generate_stripe_information() = 0;
        virtual void generate_block_repair_request() = 0;
        virtual void return_group_number() = 0;
        virtual void generate_best_placement() = 0;
        virtual void generate_sub_optimal_placement() = 0;
        void generate_random_placement(int random_seed = 10);
        
        bool check_cluster_information(std::vector<ECProject::Cluster> placement, std::map<std::string, int> placement_map);
        void generate_flat_placement();
        
    };
    class Optimal_LRC_Class : public Code_Placement
    {
    public:
        using Code_Placement::Code_Placement;
        ~Optimal_LRC_Class(){};
        bool check_parameter() override;
        int calculate_distance() override;
        void generate_best_placement() override;
        void generate_sub_optimal_placement() override;
        void nkr_to_klgr(int n, int k, int r) override;
        void klgr_to_nkr(int k, int l, int g, int r) override;

        void generate_stripe_information() override;
        void generate_block_repair_request() override;
        void return_group_number() override;
        bool encode(char **data_ptrs, char **coding_ptrs, int blocksize) override;
        bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize) override;
    };
    class Azure_LRC_Class : public Code_Placement
    {
    public:
        using Code_Placement::Code_Placement;
        ~Azure_LRC_Class(){};
        bool azure_lrc_make_matrix(int k, int g, int l, int *final_matrix);
        virtual bool encode_tansfer_plan(EncodeTransferType encode_transfer_type, TransferPlan& transfer_plan,PlacementType placement_type=PlacementType::Sub_Optimal);
        virtual std::pair<double, double> encode_tansfer_cost_balance(EncodeTransferType encode_transfer_type, PlacementType placement_type = PlacementType::Sub_Optimal);
        bool check_parameter() override;
        int calculate_distance() override;
        void generate_best_placement() override;
        void nkr_to_klgr(int n, int k, int r) override;
        void klgr_to_nkr(int k, int l, int g, int r) override;

        void generate_stripe_information() override;
        void generate_block_repair_request() override;
        void return_group_number() override;
        bool encode(char **data_ptrs, char **coding_ptrs, int blocksize) override;
        bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize) override;
        void generate_sub_optimal_placement() override;
    };
    class Azure_LRC_1_Class : public Code_Placement
    {
    public:
        using Code_Placement::Code_Placement;
        ~Azure_LRC_1_Class(){};
        bool azure_lrc_make_matrix(int k, int g, int l, int *final_matrix);
        bool check_parameter() override;
        int calculate_distance() override;
        void generate_best_placement() override;
        void generate_sub_optimal_placement() override;
        void nkr_to_klgr(int n, int k, int r) override;
        void klgr_to_nkr(int k, int l, int g, int r) override;

        void generate_stripe_information() override;
        void generate_block_repair_request() override;
        void return_group_number() override;
        bool encode(char **data_ptrs, char **coding_ptrs, int blocksize) override;
        bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize) override;
        
    };
    class Xorbas_Class : public Code_Placement
    {
    public:
        using Code_Placement::Code_Placement;
        ~Xorbas_Class(){};
        bool xorbas_make_matrix(int k, int g, int l, int *final_matrix);
        bool check_parameter() override;
        int calculate_distance() override;
        void generate_best_placement() override;
        void generate_sub_optimal_placement() override;
        void nkr_to_klgr(int n, int k, int r) override;
        void klgr_to_nkr(int k, int l, int g, int r) override;

        void generate_stripe_information() override;
        void generate_block_repair_request() override;
        void return_group_number() override;
        bool encode(char **data_ptrs, char **coding_ptrs, int blocksize) override;
        bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize) override;
    };
}

#endif