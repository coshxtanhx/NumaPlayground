cmake_minimum_required(VERSION 3.10)
project(NumaPlayground)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
add_compile_options(-Wno-interference-size -Wno-volatile)

add_executable(NPG ${SOURCES})

target_link_libraries(NPG PRIVATE /usr/lib/x86_64-linux-gnu/libnuma.so)