#include "coordinator.h"


namespace ECProject
{

grpc::Status CoordinatorImpl::setParameter(
    ::grpc::ServerContext *context,
    const coordinator_proto::Parameter *parameter,
    coordinator_proto::RepIfSetParaSucess *setParameterReply)
{
    ECSchema system_metadata(parameter->partial_decoding(),
                                (ECProject::EncodeType)parameter->encodetype(),
                                (ECProject::PlacementType)parameter->placementtype(),
                                (ECProject::EncodeTransferType)parameter->encodetransfertype(),
                                parameter->k_datablock(),
                                parameter->l_localgroup(),
                                parameter->g_m_globalparityblock());
    m_encode_parameter = system_metadata;
    setParameterReply->set_ifsetparameter(true);
    
    if (m_encode_parameter.encodetype == Xorbas)
    {
        m_encoder = std::make_shared<ECProject::Xorbas_Class>();
    }
    else if (m_encode_parameter.encodetype == Optimal_LRC)
    {
        m_encoder = std::make_shared<ECProject::Optimal_LRC_Class>();
    }
    else if (m_encode_parameter.encodetype == Azure_LRC_1)
    {
        m_encoder = std::make_shared<ECProject::Azure_LRC_1_Class>();
    }
    else
    {
        m_encoder = std::make_shared<ECProject::Azure_LRC_Class>();
    }
    m_encoder->set_parameter(-1,m_encode_parameter.k_datablock,-1,
    m_encode_parameter.g_m_globalparityblock,m_encode_parameter.l_localgroup,8);
    std::cout << "setParameter success" << std::endl;
    return grpc::Status::OK;
}
}