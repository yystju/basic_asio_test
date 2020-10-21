cmake_minimum_required(VERSION 3.9.2)

project(basic_asic_test VERSION 1.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package( Boost 1.73.0 COMPONENTS system date_time coroutine REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )


# find_package(Protobuf REQUIRED)
# include_directories(${Protobuf_INCLUDE_DIRS})
# include_directories(${CMAKE_CURRENT_BINARY_DIR})

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ./protobuff/parameter.proto)
# # protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS EXPORT_MACRO DLL_EXPORT ./protobuff/parameter.proto)

add_executable(client client.cpp trace.h graph2.h ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(client ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
set_property(TARGET client PROPERTY CXX_STANDARD 17)


add_executable(server server.cpp heijunka.h trace.h graph2.h ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(server ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
set_property(TARGET server PROPERTY CXX_STANDARD 17)

add_executable(graph2_test graph2_test.cpp graph2.h ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(graph2_test ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
set_property(TARGET graph2_test PROPERTY CXX_STANDARD 17)

add_executable(heijunka_test heijunka_test.cpp heijunka.h ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(heijunka_test ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
set_property(TARGET heijunka_test PROPERTY CXX_STANDARD 17)
