#include "MemoryManager.h"

#include <cassert> // assert
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
    assert(__bytes > sizeof(segment_t));

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

static void find_free_segment(segment_t*& segment, const memory_manager_t* memory, std::size_t size)
{
    for (auto it = memory->begin(); it != memory->end(); it = it->next())
    {
        if (!it->is_used && it->size >= size)
        {
            segment = it;
            return;
        }
    }
}

void* memory_manager_t::add_segment(std::size_t size)
{
    segment_t* segment = nullptr;
    find_free_segment(segment, this, size);

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

static void extend_segment(segment_t* extendable, const segment_t* segment)
{
    assert(extendable->next() == segment);

    extendable->size += sizeof(segment_t) + segment->size;
    extendable->is_used = false;
    segment->~segment_t();
}

static void find_segment(segment_t*& lhs, segment_t*& segment, segment_t*& rhs,
                         const memory_manager_t* memory, const void* address)
{
    segment_t* previous_it = nullptr;
    for (auto it = memory->begin(); it != memory->end(); previous_it = it, it = it->next())
    {
        if (address == it + sizeof(segment_t))
        {
            lhs = previous_it;
            segment = it;

            it = it->next();
            if (it < memory->end()) rhs = it;

            return;
        }
    }
}

bool memory_manager_t::remove_segment(void* address)
{
    segment_t* lhs = nullptr;
    segment_t* segment = nullptr;
    segment_t* rhs = nullptr;

    find_segment(lhs, segment, rhs, this, address);

    if (segment == nullptr) return false;

    segment->is_used = false;

    if (rhs != nullptr && !rhs->is_used) extend_segment(segment, rhs);
    if (lhs != nullptr && !lhs->is_used) extend_segment(lhs, segment);

    return true;
}
