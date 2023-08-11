#include "proxy.h"
bool ECProject::ProxyImpl::init_coordinator()
{
  m_coordinator_stub =
      coordinator_proto::CoordinatorService::NewStub(grpc::CreateChannel(
          m_coordinator_ip_port, grpc::InsecureChannelCredentials()));
  return true;
}
