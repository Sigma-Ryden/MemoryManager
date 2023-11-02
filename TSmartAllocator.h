#ifndef TSMART_ALLOCATOR_H
#define TSMART_ALLOCATOR_H

#include <cstddef> // size_t

template <typename T, class MemoryManager>
class TSmartAllocator {
    public:
    using value_type = T;

    public:
    T* allocate(std::size_t n, const void* hint=nullptr) {
        return (T*)MemoryManager::allocate( sizeof(T)*n );
    }

    public:
    void deallocate(T* ptr, std::size_t n) {
        MemoryManager::dellocate(ptr);
    }
};

#endif // TSMART_ALLOCATOR_H
