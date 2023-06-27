#include "General.h"
namespace REPAIR
{
    bool Azure_LRC_Class::check_parameter()
    {
        if (m_n <= m_k + m_l)
        {
            //std::cout << "Parameters do not meet requirements!" << std::endl;
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
    void Azure_LRC_Class::nkr_to_klgr(int n, int k, int r)
    {
        m_l = ceil(k, r);
        m_g = n - k - m_l;
    };
    void Azure_LRC_Class::klgr_to_nkr(int k, int l, int g, int r)
    {
        m_n = k + l + g;
    };
    void Azure_LRC_Class::generate_stripe_information()
    {
        int group_ptr = -1;
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
        std::vector<std::string> group;
        m_stripe_information.push_back(group);
        group_ptr++;
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
            for (std::string other_item : m_stripe_information[m_stripe_information.size() - 1])
            {
                if (other_item != item)
                {
                    repair_request.push_back(other_item);
                }
            }
            for(int i = 0;i < m_k - m_g + 1;i++){
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
        jerasure_matrix_encode(m_k, m_g + m_l, 8, new_matrix.data(), data_ptrs, coding_ptrs, blocksize);
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

        matrix = reed_sol_vandermonde_coding_matrix(k, g + 1, 8);
        if (matrix == NULL)
        {
            std::cout << "matrix == NULL" << std::endl;
        }

        // final_matrix = (int *)malloc(sizeof(int) * k * (g + l));
        if (final_matrix == NULL)
        {
            std::cout << "final_matrix == NULL" << std::endl;
        }
        memset(final_matrix, 0,sizeof(int) * k * (g + l));

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
}