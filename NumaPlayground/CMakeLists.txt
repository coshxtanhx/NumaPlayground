cmake_minimum_required(VERSION 3.10)
project(NumaPlayground)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
add_compile_options(-Wno-interference-size -Wno-volatile)

find_library(NUMA_LIB numactl)

if(NUMA_LIB)
    target_link_libraries(my_numa_app PRIVATE ${NUMA_LIB})
else()
    message(FATAL_ERROR "libnuma (numactl) not found!")
endif()

add_executable(NPG ${SOURCES})