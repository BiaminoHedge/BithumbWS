cmake_minimum_required(VERSION 3.10)
project(BITHUMB_LINUX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(Threads REQUIRED)

add_executable(BITHUMB
    ../../Requests/BithumbFetcher.cpp
    ../../Requests/BithumbFetcher.h
    ../../Requests/ProxyManager.cpp
    ../../Requests/ProxyManager.h
    ../../Requests/TokenParser.cpp
    ../../Requests/TokenParser.h
    ../../main.cpp
    ../../Server/WsBroadcastServer.cpp
    ../../Server/WsBroadcastServer.h
    ../../Server/WsSharedState.cpp
    ../../Server/WsSharedState.h
    ../../Server/WsListener.cpp
    ../../Server/WsListener.h
    ../../Server/WsSession.cpp
    ../../Server/WsSession.h
)

target_include_directories(BITHUMB PRIVATE
    ../../Requests
    ../../Server
)

target_link_libraries(BITHUMB PRIVATE
    CURL::libcurl
    Boost::system
    Boost::thread
    Threads::Threads
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()



