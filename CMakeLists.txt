cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(wob)

add_subdirectory(AESEngine)
add_executable(game tutorial.cxx)
target_link_libraries(game PUBLIC AES_engine)

target_include_directories(game PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/AESEngine")

