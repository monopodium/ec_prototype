cmake_minimum_required(VERSION 3.15)
project(ec_project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -DASIO_STANDALONE")

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../)
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_LIST_DIR}/../../)
set(DEBUG_PATH ${CMAKE_CURRENT_LIST_DIR}/../core_dump_debug/)

find_package(Threads REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_PATH}/third_party/grpc")
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
message(STATUS "Protobuf_INCLUDE_DIRS = ${Protobuf_INCLUDE_DIRS}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
message(STATUS "gRPC_DIR = ${gRPC_DIR}")
message(STATUS "gRPC_INCLUDE_DIRS = ${Protobuf_INCLUDE_DIRS}")
message(STATUS "gRPC_LIBS = ${gRPC_LIBS}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# Proto file
get_filename_component(cn_proto "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/coordinator.proto" ABSOLUTE)
get_filename_component(dn_proto "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/proxy.proto" ABSOLUTE)
get_filename_component(cn_proto_path "${cn_proto}" PATH)

set(cn_proto_srcs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/coordinator.pb.cc")
set(cn_proto_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/coordinator.pb.h")
set(cn_grpc_srcs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/coordinator.grpc.pb.cc")
set(cn_grpc_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/coordinator.grpc.pb.h")
set(dn_proto_srcs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/proxy.pb.cc")
set(dn_proto_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/proxy.pb.h")
set(dn_grpc_srcs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/proxy.grpc.pb.cc")
set(dn_grpc_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/include/proto/proxy.grpc.pb.h")

add_custom_command(
        OUTPUT ${cn_proto_srcs} ${cn_proto_hdrs} ${cn_grpc_srcs} ${cn_grpc_hdrs} ${dn_proto_srcs} ${dn_proto_hdrs} ${dn_grpc_srcs} ${dn_grpc_hdrs}
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_SOURCE_DIR}/include/proto"
        --cpp_out "${CMAKE_CURRENT_SOURCE_DIR}/include/proto"
        -I "${CMAKE_CURRENT_SOURCE_DIR}/include/proto"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${cn_proto}" "${dn_proto}"
        DEPENDS "${cn_proto}" "${dn_proto}")

list(APPEND INCLUDE_DIR_NOW ${Protobuf_INCLUDE_DIRS})
list(APPEND INCLUDE_DIR_NOW "${CMAKE_CURRENT_SOURCE_DIR}/include")
list(APPEND INCLUDE_DIR_NOW "${CMAKE_CURRENT_SOURCE_DIR}/include/proto")
list(APPEND INCLUDE_DIR_NOW ${THIRD_PARTY_PATH}/third_party/libmemcached/include)
list(APPEND INCLUDE_DIR_NOW ${THIRD_PARTY_PATH}/third_party/asio/include)
list(APPEND INCLUDE_DIR_NOW ${THIRD_PARTY_PATH}/third_party/tinyxml2/include)

set(ltinyxml ${THIRD_PARTY_PATH}/third_party/tinyxml2/lib/libtinyxml2.a)
set(llibmemcached ${THIRD_PARTY_PATH}/third_party/libmemcached/lib)
link_directories("${THIRD_PARTY_PATH}/third_party/libmemcached/lib")
add_library(hw_grpc_proto
${cn_proto_srcs} ${cn_proto_hdrs} ${cn_grpc_srcs} ${cn_grpc_hdrs} ${dn_proto_srcs} ${dn_proto_hdrs} ${dn_grpc_srcs} ${dn_grpc_hdrs})
target_link_libraries(hw_grpc_proto PUBLIC 
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
list(APPEND hw_grpc_proto ${cn_proto_srcs} ${cn_proto_hdrs} ${cn_grpc_srcs} ${cn_grpc_hdrs} ${dn_proto_srcs} ${dn_proto_hdrs} ${dn_grpc_srcs} ${dn_grpc_hdrs})

add_subdirectory(${TOP_DIR}/LRCs TRUE)
list(APPEND LRC_INCLUDES "${TOP_DIR}/LRCs/include")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/   PROJECT_SRC)
foreach (_target
        run_client run_coordinator run_proxy run_datanode test_tools)
    add_executable(${_target}
            ${PROJECT_SRC}
            ${CMAKE_CURRENT_SOURCE_DIR}/run_cpp/${_target}.cpp
            )
    target_include_directories(${_target} PUBLIC ${INCLUDE_DIR} ${INCLUDE_DIR_NOW} ${LRC_INCLUDES})
    target_link_libraries(${_target}
            hw_grpc_proto     
            ${_REFLECTION}
            ${_PROTOBUF_LIBPROTOBUF}
            ${_GRPC_GRPCPP}
	          ${_GRPC_GRPCPP_UNSECURE}
            ${ltinyxml}    
            -lmemcached 
            ${-llibmemcached}
            -lpthread
            LRCLibrary
            )
endforeach ()

