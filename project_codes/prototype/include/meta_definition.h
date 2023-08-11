#ifndef META_DEFINITION
#define META_DEFINITION
#include "lrc_definition.h"
#include "devcommon.h"
#include "lrc_definition.h"
#include "General.h"
#include "toolbox.h"

namespace ECProject
{
  typedef struct ClusterItem
  {
    ClusterItem() {}
    unsigned int cluster_id;
    std::string proxy_ip;
    int proxy_port;

    std::vector<unsigned int> node_ids;
  } ClusterItem;
  typedef struct NodeItem
  {
    unsigned int node_id;
    std::string node_ip;
    int node_port;
    int cluster_id;
  } NodeItem;

  typedef struct StripeItem
  {
    unsigned int Stripe_id;
    int value_size_bytes;
    int k, g_m, l_groups;
    std::vector<unsigned int> node_ids;
    EncodeType encodetype;
    PlacementType placementtype;
  } StripeItem;

  typedef struct ECSchema
  {
    ECSchema() = default;

    ECSchema(bool partial_decoding, EncodeType encodetype, PlacementType placementtype,
             EncodeTransferType encode_transfer_type, int k_datablock,
             int l_localgroup, int g_m_globalparityblock)
        : partial_decoding(partial_decoding), encodetype(encodetype), placementtype(placementtype),
          encode_transfer_type(encode_transfer_type),
          k_datablock(k_datablock), l_localgroup(l_localgroup),
          g_m_globalparityblock(g_m_globalparityblock) {}
    bool partial_decoding;
    EncodeType encodetype;
    PlacementType placementtype;
    EncodeTransferType encode_transfer_type;
    int k_datablock;
    int l_localgroup;
    int g_m_globalparityblock;
    int r_datapergoup;
  } ECSchema;  
}
#endif