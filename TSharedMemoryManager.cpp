#include <cstring>

#include "TSharedMemoryManager.h"
#include "TMemoryManager.h"

struct TSharedMemory {
    static char* Get() {
        static char* ptr = nullptr;
        if(!ptr) Init(ptr); // temp
        return ptr;
    }

    static std::size_t Size() {
        return 1024;
    }

    static bool IsUsed() {
        static auto b = false;

        auto result = b;
        if(!b) b = true;

        return result;
    }

    static void Init(char*& ptr) {
        ptr = new char[Size()]; // temp
        std::memset( ptr, 0, Size() );
    }
};

TMemoryManager& MemoryManager() {
    static TSharedMemoryManager self;
    return self;
}

void TSharedMemoryManager::init() {
    static bool inited = false;

    if(inited) return;
    inited = true;

    auto bIsUsed = TSharedMemory::IsUsed();
    MemoryManager().memory( TSharedMemory::Get(), TSharedMemory::Size() );
    if(!bIsUsed) MemoryManager().init();
}

void* TSharedMemoryManager::allocate(std::size_t bytes) {
    auto segment = MemoryManager().add_segment(bytes);
    return segment->address();
}

void TSharedMemoryManager::dellocate(void* ptr) {
    MemoryManager().remove_segment(ptr);
}

TMemorySegmentTable& TSharedMemoryManager::segment_table() {
    return *MemoryManager().segment_table();
}
