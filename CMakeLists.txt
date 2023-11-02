cmake_minimum_required(VERSION 3.5)

project(untitled LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(untitled main.cpp
    TMemoryManager.h TMemoryManager.cpp
    TSharedMemoryManager.h TSharedMemoryManager.cpp
	TSmartAllocator.h)
