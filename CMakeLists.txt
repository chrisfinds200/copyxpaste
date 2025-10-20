cmake_minimum_required(VERSION 4.0)
project(copyxpaste VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(include/clip-1.12)

add_executable(copyxpaste
        src/main.cpp
        src/core/cxp_engine.cpp
        src/core/tcp_connection.cpp
        src/core/cxp_client.cpp
        src/core/cxp_server.cpp
        src/core/connection_pool.cpp
)

target_include_directories(copyxpaste PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/clip-1.12
        ${CMAKE_SOURCE_DIR}/include/asio-asio-1-36-0/asio/include
        ${CMAKE_SOURCE_DIR}/include/copyxpaste
)

target_link_libraries(copyxpaste clip)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x00601)
    target_link_libraries(${PROJECT_NAME} ws2_32 mswsock)
endif ()