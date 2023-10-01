cmake_minimum_required(VERSION 3.8)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test testChatApp.cpp)
target_link_libraries(test ${GTEST_LIBRARIES} pthread mysqlclient crypto)

add_executable(server ../server/server.cpp)
target_link_libraries(server mysqlclient)

add_executable(client ../client/client.cpp)
target_link_libraries(client pthread crypto)