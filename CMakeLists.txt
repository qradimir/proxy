cmake_minimum_required(VERSION 3.5)
project(cpp_course)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -pedantic -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wcast-qual -Wcast-align\
        -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined -fstack-protector -lmcheck")

set(CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(UTIL_SRC
        util/composed_buffer.h
        util/log.h util/log.cpp
        util/simple_buffer.h
        util/blocking_queue.h
        util/interrupted.h
        util/executor.h)

set(NETWORK_SRC
        network/epoll.h network/epoll.cpp
        network/network_error.h
        network/file_descriptor.h network/file_descriptor.cpp
        network/server_socket.h network/server_socket.cpp
        network/client_socket.h network/client_socket.cpp
        network/ipv4.h network/ipv4.cpp
        network/event_descriptor.h network/event_descriptor.cpp
        network/timer_descriptor.h network/timer_descriptor.cpp)

set(NETWORK_HTTP_SRC
        network/http/request.h network/http/request.cpp
        network/http/request_parser.h network/http/request_parser.cpp
        network/http/parse_error.h
        network/http/msg_scanner.h network/http/msg_scanner.cpp
        network/http/read_util.h network/http/read_util.cpp)

set(ECHO_SRC
        echo/main.cpp
        echo/echo_client.h echo/echo_client.cpp
        echo/echo_server.h echo/echo_server.cpp)

set(PROXY_SRC
        proxy/main.cpp
        proxy/proxy_server.h proxy/proxy_server.cpp
        proxy/proxy_client.h proxy/proxy_client.cpp
        proxy/proxy_client_host.h proxy/proxy_client_host.cpp
        proxy/host_resolver.h proxy/host_resolver.cpp
        proxy/proxy_timer.h proxy/proxy_timer.cpp)

add_library(util STATIC ${UTIL_SRC})
set_target_properties(util PROPERTIES LINKER_LANGUAGE CXX)

add_library(network STATIC ${NETWORK_SRC})
target_link_libraries(network util)

add_library(network-http STATIC ${NETWORK_HTTP_SRC})
target_link_libraries(network-http network)

add_executable(echo ${ECHO_SRC})
target_link_libraries(echo network util)

add_executable(proxy ${PROXY_SRC})
target_link_libraries(proxy network network-http util)