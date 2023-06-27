#ifndef UTILS_H
#define UTILS_H
#include "jerasure.h"
#include "reed_sol.h"
#include "metadefinition.h"

namespace REPAIR
{
    class Cluster
    {
    public:
        Cluster(int cluster_id, int upperbound) : m_cluster_id(cluster_id), m_upperbound(upperbound)
        {
        }
        int form_group_number();
        int return_id();
        void set_upperbound(int upperbound);
        bool is_from_new_group(int group_number);
        bool isfull();
        void add_new_block(std::string new_block, int group_number);
        int remaind();
        std::vector<std::string> return_all_blocks();
        int return_block_number();

    private:
        int m_cluster_id;
        int m_upperbound;
        std::vector<std::string> m_blocks;
        std::set<int> m_from_groups;
    };

    class Code_Placement
    {
        // 类型有三：数据块，局部校验块，全局校验块
        // [[data_block1,data_block2...][],[global_parity1,global_parity1..]]
        // 数据块映射到组别
        // {"data_block":{"group:number":,"repair_request":}}
    public:
        Code_Placement(){};
        ~Code_Placement(){};
        virtual void set_debug(bool if_debug);
        virtual void print_information();
        void set_parameter(int n_in, int k_in, int r_in, int w = 8);
        virtual std::pair<double, double> return_DRC_NRC(REPAIR::PlacementType placement_type,int seed = 10);
        virtual REPAIR::Placement generate_placement(REPAIR::PlacementType placement_type, int random_seed = 10);
        // virtual void generate_best_placement() = 0;
        virtual int calculate_distance() = 0;
        virtual void nkr_to_klgr(int n, int k, int r) = 0;
        virtual void klgr_to_nkr(int k, int l, int g, int r) = 0;
        virtual bool check_parameter() = 0;
        virtual bool encode(char **data_ptrs, char **coding_ptrs, int blocksize)=0;
        virtual bool decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize)=0;
        virtual int block_to_index(std::string block);
        typedef std::vector<Cluster> PlacementRaw;
        virtual void print_placement_raw(PlacementType placement_type);
        virtual void repair_request(int index,std::vector<int> &vec);
        virtual int k_data_block_num();
        virtual int g_global_block_num();
        virtual int l_local_block_num();
        virtual int n_all_block_num();
        virtual int r_group_block_num();
        virtual bool decode_in_group_xor(int group_data_number, char **data_ptrs, char **coding_ptrs, int blocksize);
        std::string s_index_to_string(int index);
    protected:
        int m_n = 0;
        int m_k = 0;
        int m_r = 0;
        int m_l = 0;
        int m_g = 0;
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

        std::map<std::string, int> m_block_repair_cost;
        std::map<std::string, int> m_block_to_groupnumber;

        double return_NRC();
        double return_DRC();
        void generate_repair_cost(std::map<std::string, int> m_placement_map);
        std::string index_to_str(std::string block_type, int index);
        void generate_raw_information();
        virtual void generate_stripe_information() = 0;
        virtual void generate_block_repair_request() = 0;
        virtual void return_group_number() = 0;
        virtual void generate_best_placement() = 0;
        void generate_random_placement(int random_seed = 10);
        
        bool check_cluster_information(std::vector<REPAIR::Cluster> placement, std::map<std::string, int> placement_map);
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