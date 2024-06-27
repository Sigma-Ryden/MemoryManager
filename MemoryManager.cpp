#include <new> // new
#include <utility> // swap

#include "MemoryManager.h"

segment_table_t::segment_table_t(void* head, std::size_t capacity)
{
    __head = static_cast<segment_t*>(head);

    new (__head) segment_t;

    __head->address = reinterpret_cast<char*>(__head) + capacity;
    __head->size = capacity;
    __head->is_used = false;

    ++__size;
}

segment_t* segment_table_t::add_segment(std::size_t size)
{
    if (size > __head->size) return nullptr;

    auto segment = new (end()) segment_t;

    segment->address = NewAllocationAddress(size);
    segment->size = size;
    segment->is_used = true;

    ++__size;

    return segment;
}

bool segment_table_t::merge_segment(segment_t* segment)
{
    auto success = false;

    if (segment->is_used) return success;

    segment_t* lhs = nullptr;
    segment_t* rhs = nullptr;
    neighbour_segments(segment, lhs, rhs);

    if (lhs != nullptr && !lhs->is_used)
    {
        segment->address = lhs->address;
        merge_segment_implementation(segment, lhs);
        success = true;
    }
    if (rhs != nullptr && !rhs->is_used)
    {
        merge_segment_implementation(segment, rhs);
        success = true;
    }

    return success;
}

segment_t* segment_table_t::split_segment(segment_t* segment, std::size_t size)
{
    segment_t* inserted = nullptr;
    if (size < segment->size)
    {
        segment->size -= size;

        inserted = insert_segment(segment, size);
        if (inserted) inserted->is_used = false;
    }

    return inserted;
}

// success
segment_t* segment_table_t::find_segment(void* address)
{
    for (auto it = begin(); it != end(); ++it) if (it->address == address) return it;
    return nullptr;
}

// success
segment_t* segment_table_t::find_segment(std::size_t size)
{
    for (auto it = begin(); it != end(); ++it) if (!it->is_used && it->size <= size) return it;
    return nullptr;
}

// rewrite!
// extend dynamic segment after merging
void* segment_table_t::NewAllocationAddress(std::size_t size)
{
    const auto diff = __head->size - size;

    __head->size = diff;
    return reinterpret_cast<char*>(__head) + diff;
}

// a [b] c d -> a [b] c d [x] -> a [b] c [x] d -> a [b] [x] c d
segment_t* segment_table_t::insert_segment(segment_t* position, std::size_t size)
{
    for (auto it = begin(); it != end(); ++it)
    {
        if (it == position)
        {
            auto new_segment = add_segment(size);
            for (auto lhs = end() - 2, rhs = end() - 1; lhs != position; --lhs, --rhs) std::swap(*rhs, *lhs);
            return new_segment;
        }
    }
    return nullptr;
}

// a [b] c d -> a c [b] d -> a c d [b]
bool segment_table_t::remove_segment(segment_t* position)
{
    for (auto it = begin(); it != end(); ++it)
    {
        if (it == position)
        {
            for (auto lhs = position, rhs = position + 1; rhs != end(); ++lhs, ++rhs)
            {
                std::swap(*lhs, *rhs);
            }
            --__size;
            return true;
        }
    }
    return false;
}

void segment_table_t::neighbour_segments(segment_t* segment, segment_t*& lhs, segment_t*& rhs)
{
    for (auto it = begin(); it != end(); ++it)
    {
        if (it == segment)
        {
            if (it > begin()) lhs = it - 1;
            if (it < end()) rhs = it + 1;

            return;
        }
    }
}

void segment_table_t::merge_segment_implementation(segment_t* extendable, segment_t* segment)
{
    extendable->size += segment->size;
    remove_segment(segment);
}


memory_manager_t::memory_manager_t(void* memory, std::size_t size)
{
    __memory = static_cast<segment_table_t*>(memory);
    __size = size;

    auto head = reinterpret_cast<char*>(__memory) + sizeof(segment_table_t);
    auto capacity = __size - sizeof(segment_table_t);

    new (__memory) segment_table_t(head, capacity);
}

segment_t* memory_manager_t::add_segment(std::size_t size)
{
    auto segment = __memory->find_segment(size);
    if (segment)
    {
        segment = __memory->split_segment(segment, size);
        segment->is_used = true;
    }
    return segment;
}

bool memory_manager_t::remove_segment(void* address)
{
    auto segment = __memory->find_segment(address);
    if (segment)
    {
        segment->is_used = false;
        __memory->merge_segment(segment);
    }

    return segment != nullptr;
}

segment_table_t* memory_manager_t::segment_table()
{
    return __memory;
}
