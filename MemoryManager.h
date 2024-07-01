#ifndef TMEMORY_MANAGER_H
#define TMEMORY_MANAGER_H

#include <cstddef> // size_t

struct segment_t
{
public:
    std::size_t size  : sizeof(std::size_t) * 8 - 1;
    std::size_t is_used : 1;

public:
    segment_t* next() const;
};

class memory_manager_t
{
public:
    memory_manager_t(char* memory, std::size_t bytes);

public:
    segment_t* begin() const;
    segment_t* end() const;

public:
    void* add_segment(std::size_t size);
    bool remove_segment(void* address);

private:
    void find_segment(segment_t*& lhs, segment_t*& segment, segment_t*& rhs, std::size_t size);
    void find_segment(segment_t*& lhs, segment_t*& segment, segment_t*& rhs, void* address);

private:
    char* __memory = nullptr;
    std::size_t __bytes = 0;
};

#endif // TMEMORY_MANAGER_H
