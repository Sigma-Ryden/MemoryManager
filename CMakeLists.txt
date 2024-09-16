# base
cmake_minimum_required(VERSION 3.5)

project(MemoryManagerTesting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ~base

# packages
set(MemoryManager_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(MemoryManager REQUIRED)
# ~packages

# test
include_directories(test)

file(GLOB_RECURSE ${PROJECT_NAME}_TEST_SOURCES test/*.cpp)
file(GLOB_RECURSE ${PROJECT_NAME}_TEST_HEADERS test/*.hpp) # optionaly

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_TEST_SOURCES}
    ${${PROJECT_NAME}_TEST_HEADERS} # optionaly
    ${MemoryManager_HEADER_FILES} # optionaly
)
target_link_libraries(${PROJECT_NAME} MemoryManager)
# ~test
