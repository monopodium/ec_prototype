#ifndef META_DEFINITION
#define META_DEFINITION
#include "devcommon.h"
namespace REPAIR
{
    template <typename T>
    inline T ceil(T const &A, T const &B)
    {
        return T((A + B - 1) / B);
    };
    enum EncodeTransferType
    {
        Basic,
        Optimized
    };
    enum EncodeType
    {
        Xorbas,
        Azure_LRC,
        Azure_LRC_1,
        Optimal_LRC
    };
    enum PlacementType
    {
        Random,
        Flat,
        Best_Placement,
        Sub_Optimal
    };
    // namespace REPAIR
    // data globle local
    //[cluster id, cluster id
    //data [-1]globle [-1] local [-1], -1 is directly transfer
    typedef std::vector<std::vector<int>> TransferPlan;
    typedef std::vector<unsigned int> Placement;
    typedef std::map<std::string, std::vector<std::string>> RepairRequest;
    typedef struct StripeItem
    {
        std::vector<unsigned int> node_ids;
        int value_size_bytes;
        int block_size_bytes;
    } StripeItem;
    class MyRand
    {
    public:
        int operator()(int n)
        {
            struct timespec tp;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
            srand(tp.tv_nsec);
            return rand() % n;
        }
    };
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
        int return_nonparity_number();
        const std::vector<std::string> return_all_blocks();
        int return_block_number();
        const std::vector<std::string> return_parity_blocks();

    private:
        int m_cluster_id;
        int m_upperbound;
        std::vector<std::string> m_blocks;
        std::set<int> m_from_groups;
    };
    typedef std::vector<Cluster> PlacementRaw;
}

#endif