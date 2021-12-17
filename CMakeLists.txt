cmake_minimum_required(VERSION 3.5)

project(reverse_proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS program_options chrono)

add_executable(${PROJECT_NAME} "")

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    )

file(GLOB_RECURSE PRJ_SRCS src/*.cpp src/*.h src/*.hpp)
target_sources(${PROJECT_NAME} PRIVATE ${PRJ_SRCS})
target_link_libraries(${PROJECT_NAME} Boost::chrono Boost::program_options pthread rt)
