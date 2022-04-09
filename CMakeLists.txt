cmake_minimum_required(VERSION 3.5)

project(reverse_proxy LANGUAGES CXX)

add_subdirectory(boost_base)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} "")

file(GLOB_RECURSE PRJ_SRCS src/*.cpp src/*.h src/*.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_sources(${PROJECT_NAME} PRIVATE ${PRJ_SRCS})
target_link_libraries(${PROJECT_NAME} boost_base)

install (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)

install(FILES "scripts/config.txt" DESTINATION ${CMAKE_INSTALL_PREFIX}/etc/reverse_proxy/)
install(FILES "scripts/reverse_proxy.service" DESTINATION /etc/systemd/system/)
