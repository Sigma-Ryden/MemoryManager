#ifndef SEGMENT_ALLOCATOR_T_HPP
#define SEGMENT_ALLOCATOR_T_HPP

#include <cstddef> // size_t

#include "MemoryManager.h"

memory_manager_t& xxsemory_manager(); // only for test

template <typename T>
class segment_allocator_t
{
public:
    using value_type = T;

public:
    T* allocate(std::size_t n, void const* hint = nullptr)
    {
        return static_cast<T*>
        (
            xxmemory_manager().add_segment(sizeof(T) * n)
        );
    }

public:
    void deallocate(T* ptr, std::size_t n)
    {
        xxmemory_manager().remove_segment(ptr);
    }
};

#endif // SEGMENT_ALLOCATOR_T_HPP
