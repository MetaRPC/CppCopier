cmake_minimum_required(VERSION 3.15)
project(CppCopier VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH SHA256=d6c65aca6b1ed68e7a182f4757257b107ae403032760ed6ef121c9d55e81757d)
FetchContent_MakeAvailable(nlohmann_json)
include_directories(include)

add_library(CppCopier INTERFACE)
target_include_directories(CppCopier INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(quickstart examples/quickstart.cpp)
target_link_libraries(CppCopier INTERFACE winhttp nlohmann_json::nlohmann_json)
target_link_libraries(quickstart PRIVATE CppCopier)

add_executable(list_copiers examples/list.cpp)
target_link_libraries(list_copiers PRIVATE CppCopier)

enable_testing()
add_executable(contract_tests tests/contract.cpp)
target_link_libraries(contract_tests PRIVATE CppCopier)
add_test(NAME contract COMMAND contract_tests)

add_executable(live_adapter examples/live_adapter.cpp)
target_link_libraries(live_adapter PRIVATE CppCopier)
