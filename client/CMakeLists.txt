cmake_minimum_required(VERSION 3.8)

add_executable(client client.cpp)
target_link_libraries(client pthread crypto)
