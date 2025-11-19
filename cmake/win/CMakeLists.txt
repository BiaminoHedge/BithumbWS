cmake_minimum_required(VERSION 3.10)
project(BITHUMB)

set(CMAKE_CXX_STANDARD 14)

find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(BITHUMB
        Requests/BithumbFetcher.cpp
        Requests/BithumbFetcher.h
        Requests/ProxyManager.cpp
        Requests/ProxyManager.h
        Requests/TokenParser.cpp
        Requests/TokenParser.h
        main.cpp
        Server/WsBroadcastServer.cpp
        Server/WsBroadcastServer.h
        Server/WsSharedState.cpp
        Server/WsSharedState.h
        Server/WsListener.cpp
        Server/WsListener.h
        Server/WsSession.cpp
        Server/WsSession.h
)

target_include_directories(BITHUMB PRIVATE Requests)
target_link_libraries(BITHUMB PRIVATE CURL::libcurl Boost::system Boost::thread)

if (MSVC)
    # Исправляем C1128
    target_compile_options(BITHUMB PRIVATE /bigobj /utf-8)
    target_compile_definitions(BITHUMB PRIVATE _WIN32_WINNT=0x0A00 WINVER=0x0A00)
    target_link_libraries(BITHUMB PRIVATE ws2_32 mswsock)
elseif (MINGW)
    # Фикс связанные boost без этого не работал
    target_compile_options(BITHUMB PRIVATE -Wa,-mbig-obj)
    # Аналогично
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g1 -O1")
    # Winsock для Boost.Asio
    target_link_libraries(BITHUMB PRIVATE ws2_32 mswsock)
endif()

