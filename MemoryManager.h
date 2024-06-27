#ifndef TMEMORY_MANAGER_H
#define TMEMORY_MANAGER_H

#include <cstddef> // size_t

struct segment_t
{
    void* address = nullptr;
    std::size_t size  : sizeof(std::size_t) * sizeof(nullptr) - 1;
    std::size_t is_used : 1;
};

class segment_table_t
{
public:
    segment_table_t(void* root, std::size_t capacity);

public:
    segment_t* add_segment(std::size_t size);
    bool merge_segment(segment_t* segment);
    segment_t* split_segment(segment_t* segment, std::size_t size);

public:
    segment_t* find_segment(void* address);
    segment_t* find_segment(std::size_t size);

public:
    segment_t* begin() const { return __head; }
    segment_t* end() const { return begin() + __size; }

protected:
    void* NewAllocationAddress(std::size_t size);

protected:
    segment_t* insert_segment(segment_t* position, std::size_t size);
    bool remove_segment(segment_t* position);
    void neighbour_segments(segment_t* segment, segment_t*& lhs, segment_t*& rhs);
    void merge_segment_implementation(segment_t* extendable, segment_t* segment);

protected:
    segment_t* __head = nullptr;
    std::size_t __size = 0;
};

class memory_manager_t
{
public:
    memory_manager_t(void* memory, std::size_t size);

public:
    segment_t* add_segment(std::size_t size);
    bool remove_segment(void* address);

public:
    segment_table_t* segment_table();

protected:
    segment_table_t* __memory = nullptr;
    std::size_t __size = 0;
};

#endif // TMEMORY_MANAGER_H
