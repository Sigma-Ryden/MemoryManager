#include "MemoryManager.h"

#include <new> // placement new

segment_t* segment_t::next() const
{
    return reinterpret_cast<segment_t*>
    (
        reinterpret_cast<char*>(const_cast<segment_t*>(this)) + sizeof(segment_t) + size
    );
}

memory_manager_t::memory_manager_t(char* memory, std::size_t bytes)
    : __memory(memory), __bytes(bytes)
{
    auto segment = new (__memory) segment_t;
    segment->size = __bytes - sizeof(segment_t);
    segment->is_used = false;
}

segment_t* memory_manager_t::begin() const
{
    return reinterpret_cast<segment_t*>(__memory);
}

segment_t* memory_manager_t::end() const
{
    return reinterpret_cast<segment_t*>(__memory + __bytes);
}

void* memory_manager_t::add_segment(std::size_t size)
{
    segment_t* lhs = nullptr;
    segment_t* segment = nullptr;
    segment_t* rhs = nullptr;

    find_segment(lhs, segment, rhs, size);

    if (segment == nullptr) return nullptr;

    if (segment->size == size)
    {
        segment->is_used = true;
    }
    else if (segment->size > size + sizeof(segment_t))
    {
        auto diff = segment->size - size;

        segment->size = size;
        segment->is_used = true;

        auto created = new (segment->next()) segment_t;
        created->size = diff - sizeof(segment_t);
        created->is_used = false;
    }
    else
    {
        return nullptr;
    }

    return segment + sizeof(segment_t);
}

bool memory_manager_t::remove_segment(void* address)
{
    bool is_removed = false;

    segment_t* lhs = nullptr;
    segment_t* segment = nullptr;
    segment_t* rhs = nullptr;

    find_segment(lhs, segment, rhs, address);

    if (segment == nullptr) return is_removed;
    else segment->is_used = false;

    if (rhs != nullptr && !rhs->is_used)
    {
        segment->size += sizeof(segment_t) + rhs->size;
        segment->is_used = false;
        rhs->~segment_t();
        is_removed = true;
    }
    if(lhs != nullptr && !lhs->is_used)
    {
        lhs->size += sizeof(segment_t) + segment->size;
        lhs->is_used = false;
        segment->~segment_t();
        is_removed = true;
    }

    return is_removed;
}

void memory_manager_t::find_segment(segment_t*& lhs, segment_t*& segment, segment_t*& rhs, std::size_t size)
{
    segment_t* previous_it = nullptr;
    for (auto it = begin(); it != end(); previous_it = it, it = it->next())
    {
        if (!it->is_used && it->size >= size)
        {
            lhs = previous_it;
            segment = it;

            it = it->next();
            if (it < end()) rhs = it;

            return;
        }
    }
}

void memory_manager_t::find_segment(segment_t*& lhs, segment_t*& segment, segment_t*& rhs, void* address)
{
    segment_t* previous_it = nullptr;
    for (auto it = begin(); it != end(); previous_it = it, it = it->next())
    {
        if (address == it + sizeof(segment_t))
        {
            lhs = previous_it;
            segment = it;

            it = it->next();
            if (it < end()) rhs = it;

            return;
        }
    }
}
