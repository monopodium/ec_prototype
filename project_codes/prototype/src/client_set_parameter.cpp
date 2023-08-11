#include "client.h"
#include "coordinator.grpc.pb.h"

bool ECProject::Client::set_parameter_grpc(ECSchema input_ecschema)
{
  coordinator_proto::Parameter parameter;
  parameter.set_encodetype((int)input_ecschema.encodetype);
  parameter.set_placementtype(input_ecschema.placementtype);
  parameter.set_encodetransfertype(input_ecschema.encode_transfer_type);
  parameter.set_k_datablock(input_ecschema.k_datablock);
  parameter.set_l_localgroup(input_ecschema.l_localgroup);
  parameter.set_g_m_globalparityblock(input_ecschema.g_m_globalparityblock);
  grpc::ClientContext context;
  coordinator_proto::RepIfSetParaSucess reply;
  grpc::Status status = m_coordinator_ptr->setParameter(&context, parameter, &reply);
  if (status.ok())
  {
    return reply.ifsetparameter();
  }
  else
  {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    return false;
  }
  return true;
}
