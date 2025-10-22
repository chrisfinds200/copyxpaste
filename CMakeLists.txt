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

target_link_libraries(copyxpaste PRIVATE clip)

if (WIN32)
    target_compile_definitions(copyxpaste PRIVATE _WIN32_WINNT=0x0A00)

    # Winsock libs for networking
    target_link_libraries(copyxpaste PRIVATE ws2_32 mswsock)

    if (MINGW)
        # Avoid libgcc_s_seh-1.dll and libstdc++-6.dll
        target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
    endif()
endif ()
