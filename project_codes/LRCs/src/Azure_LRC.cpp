#include "General.h"
namespace ECProject
{
    bool Azure_LRC_Class::check_parameter()
    {
        if (m_n <= m_k + m_l)
        {
            // std::cout << "Parameters do not meet requirements!" << std::endl;
            return false;
        }
        if (m_n<=0||m_k<=0||m_r<=0||m_l<=0||m_g<=0){
            return false;
        }
        return true;
    };
    int Azure_LRC_Class::calculate_distance()
    {
        nkr_to_klgr(m_n, m_k, m_r);
        m_d = m_g + 2;
        return m_d;
    };
    /**
        void Azure_LRC_Class::generate_best_placement()
        {
            int b = m_d - 1;
            for (int i = 0; i < m_n; i++)
            {
                m_best_placement_raw.push_back(Cluster(i, b));
            }
            for (int i = 0; i < m_l; i++)
            {
                std::vector<std::string> group = m_stripe_information[i];
                int cur_group_len = group.size();
                if (b >= cur_group_len)
                {
                    for (Cluster &each_cluster : m_best_placement_raw)
                    {
                        // /
                        if (int(each_cluster.is_from_new_group(i)) + each_cluster.form_group_number() + m_g >= each_cluster.return_block_number() + cur_group_len)
                        {
                            for (std::string block : group)
                            {
                                each_cluster.add_new_block(block, i);
                                m_best_placement_map[block] = each_cluster.return_id();
                            }
                            break;
                        }
                    }
                }
                else
                {
                    int cluster_number = ceil(cur_group_len, b);
                    for (int j = 0; j < cluster_number; j++)
                    {
                        int number_in_group = std::min(b, cur_group_len - j * b);
                        for (Cluster &each_cluster : m_best_placement_raw)
                        {
                            if (int(each_cluster.is_from_new_group(i)) + each_cluster.form_group_number() + m_g >=
                                each_cluster.return_block_number() + number_in_group)
                            {
                                for (int ii = 0; ii < number_in_group; ii++)
                                {
                                    each_cluster.add_new_block(group[j * b + ii], i);
                                    m_best_placement_map[group[j * b + ii]] = each_cluster.return_id();
                                }
                                break;
                            }
                        }
                    }
                }
            }
            for (Cluster &each_cluster : m_best_placement_raw)
            {
                if (each_cluster.return_block_number() == 0)
                {
                    for (std::string block : m_stripe_information[m_stripe_information.size() - 1])
                    {
                        each_cluster.add_new_block(block, m_stripe_information.size() - 1);
                        m_best_placement_map[block] = each_cluster.return_id();
                    }
                    break;
                }
            }
            for (int i = 0; i < m_n; i++)
            {
                if (m_best_placement_raw[i].return_block_number() == 0)
                {
                    m_best_placement_raw.erase(m_best_placement_raw.begin() + i);
                }
            }
        };
    **/

    void Azure_LRC_Class::generate_sub_optimal_placement()
    {
        for (int i = 0; i < m_n; i++)
        {
            m_sub_optimal_placement_raw.push_back(Cluster(i, m_d - 1));
        }
        int p_cluster = 0;
        if (m_g >= m_r)
        {
            for (size_t i = 0; i < m_stripe_information.size() - 1; i++)
            {
                std::vector<std::string> group = m_stripe_information[i];
                if (m_sub_optimal_placement_raw[p_cluster].return_nonparity_number() + group.size() - 1 > m_g)
                {

                    p_cluster++;
                }
                for (size_t j = 0; j < group.size() - 1; j++)
                {
                    std::string block = group[j];
                    int group_number = m_block_to_groupnumber[block];
                    int cluster_id = m_sub_optimal_placement_raw[p_cluster].return_id();
                    m_sub_optimal_placement_raw[p_cluster].add_new_block(block, group_number);
                    m_sub_optimal_placement_map[block] = cluster_id;
                }
            }
        }
        else
        {
            std::vector<std::vector<std::string> > remain_list;
            for (size_t i = 0; i < m_stripe_information.size() - 1; i++)
            {
                std::vector<std::string> group = m_stripe_information[i];
                int remain = group.size() - 1;
                while (remain >= m_d - 1)
                {
                    for (int j = 0; j < m_d - 1; j++)
                    {
                        std::string block = group[group.size() - 1 - remain + j];
                        int group_number = m_block_to_groupnumber[block];
                        int cluster_id = m_sub_optimal_placement_raw[p_cluster].return_id();
                        m_sub_optimal_placement_raw[p_cluster].add_new_block(block, group_number);
                        m_sub_optimal_placement_map[block] = cluster_id;
                    }
                    remain = remain - (m_d - 1);
                    p_cluster++;
                }
                if (remain > 0)
                {
                    int begin = group.size() - 1 - remain;
                    int end = group.size() - 1;
                    std::vector<std::string> Arrs2(group.begin() + begin, group.begin() + end);
                    remain_list.push_back(Arrs2);
                }
            }
            for (size_t j = 0; j < remain_list.size(); j++)
            {
                if (m_sub_optimal_placement_raw[p_cluster].return_nonparity_number() + remain_list[j].size() > m_g)
                {
                    p_cluster++;
                }
                for (auto block : remain_list[j])
                {
                    int group_number = m_block_to_groupnumber[block];
                    int cluster_id = m_sub_optimal_placement_raw[p_cluster].return_id();
                    m_sub_optimal_placement_raw[p_cluster].add_new_block(block, group_number);
                    m_sub_optimal_placement_map[block] = cluster_id;
                }
            }
        }
        std::vector<std::string> global_parity = m_stripe_information[m_stripe_information.size() - 1];
        for (size_t i = 0; i < m_sub_optimal_placement_raw.size(); i++)
        {
            Cluster *each_cluster = &m_sub_optimal_placement_raw[i];
            if (each_cluster->return_block_number() == 0)
            {
                for (std::string block : global_parity)
                {
                    int group_number = m_block_to_groupnumber[block];
                    int cluster_id = each_cluster->return_id();
                    each_cluster->add_new_block(block, group_number);
                    m_sub_optimal_placement_map[block] = cluster_id;
                }
                break;
            }
        }

        for (size_t i = 0; i < m_sub_optimal_placement_raw.size(); i++)
        {
            if (m_sub_optimal_placement_raw[i].return_block_number() == 0)
            {
                p_cluster = i;
                break;
            }
        }
        for (int i = 0; i < m_stripe_information.size() - 1; i++)
        {
            std::string last_data = m_stripe_information[i][m_stripe_information[i].size() - 2];
            std::string local_block = m_stripe_information[i][m_stripe_information[i].size() - 1];
            int last_cluster_number = m_sub_optimal_placement_map[last_data];
            int input_cluster_ptr = last_cluster_number;
            if (m_sub_optimal_placement_raw[last_cluster_number].return_nonparity_number() > m_g)
            {
                input_cluster_ptr = p_cluster;
            }
            int group_number = m_block_to_groupnumber[local_block];
            m_sub_optimal_placement_raw[input_cluster_ptr].add_new_block(local_block, group_number);
            m_sub_optimal_placement_map[local_block] = input_cluster_ptr;
        }

        for (int i = 0; i < m_n; i++)
        {
            if (m_sub_optimal_placement_raw[i].return_block_number() == 0)
            {
                m_sub_optimal_placement_raw.erase(m_sub_optimal_placement_raw.begin() + i);
            }
        }
        if (!check_cluster_information(m_sub_optimal_placement_raw, m_sub_optimal_placement_map))
        {
            std::cout << "!!!best Azure_LRC check_cluster_information() error" << std::endl;
        }
    };

    void Azure_LRC_Class::generate_best_placement()
    {
        for (int i = 0; i < m_n; i++)
        {
            m_best_placement_raw.push_back(Cluster(i, m_d - 1));
        }
        int p_cluster = 0;
        if (m_g >= m_r)
        {
            for (size_t i = 0; i < m_stripe_information.size() - 1; i++)
            {
                std::vector<std::string> group = m_stripe_information[i];
                if (m_best_placement_raw[p_cluster].return_nonparity_number() + group.size() - 1 > m_g)
                {

                    p_cluster++;
                }
                for (size_t j = 0; j < group.size() - 1; j++)
                {
                    std::string block = group[j];
                    int group_number = m_block_to_groupnumber[block];
                    int cluster_id = m_best_placement_raw[p_cluster].return_id();
                    m_best_placement_raw[p_cluster].add_new_block(block, group_number);
                    m_best_placement_map[block] = cluster_id;
                }
            }
        }
        else
        {
            std::vector<std::vector<std::string> > remain_list;
            for (size_t i = 0; i < m_stripe_information.size() - 1; i++)
            {
                std::vector<std::string> group = m_stripe_information[i];
                int remain = group.size() - 1;
                while (remain >= m_d - 1)
                {
                    for (int j = 0; j < m_d - 1; j++)
                    {
                        std::string block = group[group.size() - 1 - remain + j];
                        int group_number = m_block_to_groupnumber[block];
                        int cluster_id = m_best_placement_raw[p_cluster].return_id();
                        m_best_placement_raw[p_cluster].add_new_block(block, group_number);
                        m_best_placement_map[block] = cluster_id;
                    }
                    remain = remain - (m_d - 1);
                    p_cluster++;
                }
                if (remain > 0)
                {
                    int begin = group.size() - 1 - remain;
                    int end = group.size() - 1;
                    std::vector<std::string> Arrs2(group.begin() + begin, group.begin() + end);
                    remain_list.push_back(Arrs2);
                }
            }
            for (size_t j = 0; j < remain_list.size(); j++)
            {
                if (m_best_placement_raw[p_cluster].return_nonparity_number() + remain_list[j].size() > m_g)
                {
                    p_cluster++;
                }
                for (auto block : remain_list[j])
                {
                    int group_number = m_block_to_groupnumber[block];
                    int cluster_id = m_best_placement_raw[p_cluster].return_id();
                    m_best_placement_raw[p_cluster].add_new_block(block, group_number);
                    m_best_placement_map[block] = cluster_id;
                }
            }
        }
        std::vector<std::string> global_parity = m_stripe_information[m_stripe_information.size() - 1];
        int remain = global_parity.size();
        int global_ptr = 0;
        for (size_t i = 0; i < m_best_placement_raw.size(); i++)
        {
            Cluster *each_cluster = &m_best_placement_raw[i];
            while (each_cluster->return_nonparity_number() < m_g)
            {
                std::string block = global_parity[global_ptr];
                int group_number = m_block_to_groupnumber[block];
                int cluster_id = each_cluster->return_id();
                each_cluster->add_new_block(block, group_number);
                m_best_placement_map[block] = cluster_id;
                global_ptr = global_ptr + 1;
                if (global_ptr == remain)
                {
                    break;
                }
            }
            if (global_ptr == remain)
            {
                break;
            }
        }

        for (size_t i = 0; i < m_best_placement_raw.size(); i++)
        {
            if (m_best_placement_raw[i].return_block_number() == 0)
            {
                p_cluster = i;
                break;
            }
        }
        for (int i = 0; i < m_stripe_information.size() - 1; i++)
        {
            std::string last_data = m_stripe_information[i][m_stripe_information[i].size() - 2];
            std::string local_block = m_stripe_information[i][m_stripe_information[i].size() - 1];
            int last_cluster_number = m_best_placement_map[last_data];
            int input_cluster_ptr = last_cluster_number;
            if (m_best_placement_raw[last_cluster_number].return_nonparity_number() > m_g)
            {
                input_cluster_ptr = p_cluster;
            }
            int group_number = m_block_to_groupnumber[local_block];
            m_best_placement_raw[input_cluster_ptr].add_new_block(local_block, group_number);
            m_best_placement_map[local_block] = input_cluster_ptr;
        }

        for (int i = 0; i < m_n; i++)
        {
            if (m_best_placement_raw[i].return_block_number() == 0)
            {
                m_best_placement_raw.erase(m_best_placement_raw.begin() + i);
            }
        }
        if (!check_cluster_information(m_best_placement_raw, m_best_placement_map))
        {
            std::cout << "!!!best Azure_LRC check_cluster_information() error" << std::endl;
        }
    };

    void Azure_LRC_Class::nkr_to_klgr(int n, int k, int r)
    {
        m_l = ceil(k, r);
        m_g = n - k - m_l;
    };
    void Azure_LRC_Class::klgr_to_nkr(int k, int l, int g, int r)
    {
        m_n = k + l + g;
        m_r = r;
    };
    void Azure_LRC_Class::generate_stripe_information()
    {
        int group_ptr = -1;
        int data_block_ptr = 0;

        std::vector<int> each_r(m_l, 0);


        if(m_l_stable){
            for(int i = 0;i < m_k;i++){
                each_r[i%m_l]++;
            }
            std::vector<int>::iterator maxElement = std::max_element(each_r.begin(), each_r.end());
            m_r = *maxElement;
            for(int i = 0; i<m_l;i++){
                std::vector<std::string> group;
                for(int j = 0;j<each_r[i];j++){
                    std::string block = index_to_str("D", data_block_ptr);
                    group.push_back(block);
                    m_block_to_groupnumber[block] = i;
                    data_block_ptr++;
                    
                }
                group_ptr++;
                m_stripe_information.push_back(group);
            }
        }else{
            for (int i = 0; i < m_k; i++)
            {
                std::string block = index_to_str("D", i);
                if (i % m_r == 0)
                {
                    std::vector<std::string> group;
                    m_stripe_information.push_back(group);
                    group_ptr++;
                }
                m_stripe_information[int(i / m_r)].push_back(block);
                m_block_to_groupnumber[block] = group_ptr;
            }
            m_l = group_ptr + 1;
        }
        std::vector<std::string> group;
        m_stripe_information.push_back(group);
        group_ptr++;
        std::cout<<"group_ptr:"<<group_ptr<<std::endl;
        std::cout<<"m_g"<<m_g<<std::endl;
        for (int i = 0; i < m_g; i++)
        {
            std::string block = index_to_str("G", i);
            m_stripe_information[group_ptr].push_back(block);
            m_block_to_groupnumber[block] = group_ptr;
        }
        for (int i = 0; i < m_l; i++)
        {
            std::string block = index_to_str("L", i);
            m_stripe_information[i].push_back(block);
            m_block_to_groupnumber[block] = i;
        }
    };
    void Azure_LRC_Class::generate_block_repair_request()
    {
        for (int i = 0; i < m_l; i++)
        {
            for (std::string item : m_stripe_information[i])
            {
                std::vector<std::string> repair_request;
                for (std::string other_item : m_stripe_information[i])
                {
                    if (other_item != item)
                    {
                        repair_request.push_back(other_item);
                    }
                }
                m_block_repair_request[item] = repair_request;
            }
        }
        for (std::string item : m_stripe_information[m_stripe_information.size() - 1])
        {
            std::vector<std::string> repair_request;
            // for (std::string other_item : m_stripe_information[m_stripe_information.size() - 1])
            // {
            //     if (other_item != item)
            //     {
            //         repair_request.push_back(other_item);
            //     }
            // }
            for (int i = 0; i < m_k; i++)
            {
                repair_request.push_back(s_index_to_string(i));
            }
            m_block_repair_request[item] = repair_request;
        }
    };
    void Azure_LRC_Class::return_group_number(){};
    bool Azure_LRC_Class::encode(char **data_ptrs, char **coding_ptrs, int blocksize)
    {
        std::vector<int> new_matrix((m_g + m_l) * m_k, 0);
        azure_lrc_make_matrix(m_k, m_g, m_l, new_matrix.data());
        jerasure_matrix_encode(m_k, m_g + m_l, m_w, new_matrix.data(), data_ptrs, coding_ptrs, blocksize);
        return true;
    };
    bool Azure_LRC_Class::decode(char **data_ptrs, char **coding_ptrs, int *erasures, int blocksize)
    {
        return true;
    }
    bool Azure_LRC_Class::azure_lrc_make_matrix(int k, int g, int l, int *final_matrix)
    {
        int r = (k + l - 1) / l;
        int *matrix = NULL;

        matrix = reed_sol_vandermonde_coding_matrix(k, g + 1, m_w);
        if (matrix == NULL)
        {
            std::cout << "matrix == NULL" << std::endl;
        }

        // final_matrix = (int *)malloc(sizeof(int) * k * (g + l));
        if (final_matrix == NULL)
        {
            std::cout << "final_matrix == NULL" << std::endl;
        }
        bzero(final_matrix, sizeof(int) * k * (g + l));

        for (int i = 0; i < g; i++)
        {
            for (int j = 0; j < k; j++)
            {
                final_matrix[i * k + j] = matrix[(i + 1) * k + j];
            }
        }

        for (int i = 0; i < l; i++)
        {
            for (int j = 0; j < k; j++)
            {
                if (i * r <= j && j < (i + 1) * r)
                {
                    final_matrix[(i + g) * k + j] = 1;
                }
            }
        }

        free(matrix);
        return true;
    }

    bool Azure_LRC_Class::encode_tansfer_plan(EncodeTransferType encode_transfer_type, TransferPlan &transfer_plan, PlacementType placement_type)
    {
        ECProject::Placement placement_plan = generate_placement(placement_type);
        // for(int i = 0;i<placement_plan.size();i++){
        //     std::cout<<"placement_plan[i]:"<<i<<"  "<<placement_plan[i]<<std::endl;
        // }
        return_DRC_NRC(placement_type);
        for (int i = 0; i < m_k + m_g; i++)
        {
            std::vector<int> plan_for_i(1, -1);
            plan_for_i.push_back(placement_plan[i]);
            transfer_plan.push_back(plan_for_i);
        }
        for (int i = 0; i < m_l; i++)
        {
            std::vector<int> plan_for_i;
            if (m_block_repair_cost[index_to_str("L", i)] >= 2 || encode_transfer_type == EncodeTransferType::Basic)
            {
                std::vector<int> plan_for_i(1, -1);
                plan_for_i.push_back(placement_plan[m_k + m_g + i]);
                transfer_plan.push_back(plan_for_i);
            }
            else
            {
                for (std::string request_block : m_stripe_information[i])
                {
                    int request_block_index = block_to_index(request_block);
                    if (request_block_index != m_k + m_g + i)
                    {
                        plan_for_i.push_back(request_block_index);
                    }
                }
                plan_for_i.push_back(-1);
                plan_for_i.push_back(placement_plan[m_k + m_g + i]);
                transfer_plan.push_back(plan_for_i);
            }
        }
        return true;
    }
    // balance_ratio = (traffic_max - traffic_avg) / traffic_avg
    //  traffic_max = client和cluster之间最大的traffic
    //
    std::pair<double, double> Azure_LRC_Class::encode_tansfer_cost_balance(EncodeTransferType encode_transfer_type, PlacementType placement_type)
    {
        ECProject::TransferPlan transfer_plan;
        encode_tansfer_plan(encode_transfer_type, transfer_plan);
        double cost = 0;
        double traffic_balance_ratio = 0;
        for (int i = 0; i < int(transfer_plan.size()); i++)
        {
            if (transfer_plan[i][0] == -1)
            {
                cost++;
            }
            else
            {
                cost = cost + m_block_repair_cost[s_index_to_string(i)];
            }
        }
        PlacementRaw placement_raw = select_placement_raw(placement_type);
        std::vector<int> traffic_list;
        for (size_t i = 0; i < placement_raw.size(); i++)
        {
            Cluster each_cluster = placement_raw[i];
            std::vector<std::string> blocks_in_cluster = each_cluster.return_all_blocks();
            int traffic_this_cluster = 0;
            for(std::string each_block:blocks_in_cluster){
                if(transfer_plan[block_to_index(each_block)][0]==-1){
                    traffic_this_cluster++;
                }
            }
            if(traffic_this_cluster!=0){
                traffic_list.push_back(traffic_this_cluster);
            }
        }
        // if (encode_transfer_type == EncodeTransferType::Optimized)
        // {
        //     for (int i = 0; i < m_l; i++)
        //     {
        //         if (m_block_repair_cost[index_to_str("L", i)] == 1)
        //         {
        //             traffic_list.push_back(1);
        //         }
        //     }
        // }
        int traffic_max = 0;
        double traffic_avg = 0;
        int traffic_sum = 0;
        for (int traffic_item : traffic_list)
        {
            traffic_sum += traffic_item;
            if (traffic_item > traffic_max)
            {
                traffic_max = traffic_item;
            }
        }
        std::cout << "traffic_list" << std::endl;
        for (int i = 0; i < traffic_list.size(); i++)
        {
            std::cout << traffic_list[i] << " " << std::endl;
        }
        traffic_avg = double(traffic_sum) / double(traffic_list.size());
        traffic_balance_ratio = double(traffic_max - traffic_avg) / double(traffic_avg);
        // std::cout << "traffic_avg:          " << traffic_avg << std::endl;
        // std::cout << "traffic_max:          " << traffic_max << std::endl;
        // std::cout << "traffic_balance_ratio:" << traffic_balance_ratio << std::endl;
        return std::make_pair(cost, traffic_balance_ratio);
    }
}