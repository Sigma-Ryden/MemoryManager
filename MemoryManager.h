#ifndef TMEMORY_MANAGER_H
#define TMEMORY_MANAGER_H

#include <cstddef> // size_t

struct segment_t
{
    std::size_t size  : sizeof(std::size_t) * 8 - 1;
    std::size_t is_used : 1;

    void* memory();
    segment_t* next();
};

class memory_manager_t
{
public:
    memory_manager_t(char* memory, std::size_t bytes);

public:
    void* add_segment(std::size_t size);
    bool remove_segment(void* address);

public:
    segment_t* begin() const { return __begin; }
    segment_t* end() const { return __end; }

private:
    segment_t* __begin = nullptr;
    segment_t* __end = nullptr;
};

#endif // TMEMORY_MANAGER_H
