#include "proxy.h"
grpc::Status ECProject::ProxyImpl::checkalive(grpc::ServerContext *context,
                                     const proxy_proto::CheckaliveCMD *request,
                                     proxy_proto::RequestResult *response)
{
    std::cout << "checkalive" << request->name() << std::endl;
    response->set_message(false);
    init_coordinator();
    return grpc::Status::OK;
}