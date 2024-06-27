#ifndef SEGMENT_ALLOCATOR_T_HPP
#define SEGMENT_ALLOCATOR_T_HPP

#include <cstddef> // size_t

template <typename T>
class segment_allocator_t
{
public:
    using value_type = T;

public:
    T* allocate(std::size_t n, const void* hint = nullptr)
    {
        return nullptr;
    }

public:
    void deallocate(T* ptr, std::size_t n)
    {
    }
};

#endif // SEGMENT_ALLOCATOR_T_HPP
