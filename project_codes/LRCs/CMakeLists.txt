cmake_minimum_required(VERSION 3.15)
project(LRCs)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -DASIO_STANDALONE")

#the directory for the third parity
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party)
option(LRC_TEST_BUILD "Build doc target for LRC project" OFF)

#add the pybind
add_subdirectory(../../third_party/pybind11 "${THIRD_PARTY_DIR}/pybind11")
set(simulator_source
        pybind_cpp/interface.cpp
        pybind_cpp/interface.h
        pybind_cpp/pybind.cpp)
pybind11_add_module(LRCs_simulator ${simulator_source})

#set the include directory for third parity
list(APPEND THIRD_INCLUDE ${THIRD_PARTY_DIR}/gf-complete/include)
list(APPEND THIRD_INCLUDE ${THIRD_PARTY_DIR}/jerasure/include)
list(APPEND THIRD_INCLUDE ${THIRD_PARTY_DIR}/jerasure/include/jerasure)
list(APPEND THIRD_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(Jerasurelib ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/jerasure/lib/libJerasure.so)
set(gf_completelib ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/gf-complete/lib/libgf_complete.so)

# link_directories(${THIRD_PARTY_DIR}/jerasure/lib)
# link_directories(${THIRD_PARTY_DIR}/gf-complete/lib)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/   PROJECT_SRC)

add_library(LRCLibrary SHARED ${PROJECT_SRC})

target_link_libraries(LRCLibrary
${Jerasurelib} ${gf_completelib}
)
target_include_directories(LRCLibrary PUBLIC ${THIRD_INCLUDE})

set_target_properties(LRCLibrary PROPERTIES output_name "LRCLibrary")

if(LRC_TEST_BUILD)
foreach (_target
        test_tools)
    add_executable(${_target}            
            
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Azure_LRC_1.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Azure_LRC.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/General.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Optimal_LRC.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Xorbas.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/${_target}.cpp
            )
    target_link_libraries(${_target}
                ${Jerasurelib} ${gf_completelib}
            )
endforeach ()
endif()






target_compile_definitions(LRCs_simulator
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_link_libraries(LRCs_simulator
PUBLIC
${Jerasurelib} ${gf_completelib} LRCLibrary
)
