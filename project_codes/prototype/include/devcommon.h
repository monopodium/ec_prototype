#ifndef DEVCOMMON
#define DEVCOMMON

#include <iostream>
#include <map>
#include <memory>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <set>
#include <asio.hpp>
#include <tinyxml2.h>
#include <libmemcached/memcached.h>

#ifdef BAZEL_BUILD
#include "src/proto/coordinator.grpc.pb.h"
#else
#include "coordinator.grpc.pb.h"
#endif


#include <grpcpp/grpcpp.h>

#include "proxy.grpc.pb.h"
#include <grpc++/create_channel.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include <grpcpp/health_check_service_interface.h>


#endif

