cmake_minimum_required(VERSION 3.15)
project(CppCopier VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(CppCopier INTERFACE)
target_include_directories(CppCopier INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(quickstart examples/quickstart.cpp)
target_link_libraries(CppCopier INTERFACE winhttp)
target_link_libraries(quickstart PRIVATE CppCopier)

add_executable(list_copiers examples/list.cpp)
target_link_libraries(list_copiers PRIVATE CppCopier)

enable_testing()
add_executable(contract_tests tests/contract.cpp)
target_link_libraries(contract_tests PRIVATE CppCopier)
add_test(NAME contract COMMAND contract_tests)
