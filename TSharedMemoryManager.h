#ifndef TSHARED_MEMORY_MANAGER_H
#define TSHARED_MEMORY_MANAGER_H

#include <cstddef> // size_t

#include "TMemoryManager.h"

class TSharedMemoryManager : public TMemoryManager {
    public:
    static void init();

    public:
    static void* allocate(std::size_t bytes);
    static void dellocate(void* ptr);

    public:
    static TMemorySegmentTable& segment_table();
};

#endif // TSHARED_MEMORY_MANAGER_H
