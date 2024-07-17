#include "MemoryManager.h"

#include <cassert> // assert
#include <new> // placement new

void* segment_t::memory()
{
    return reinterpret_cast<char*>(this) + sizeof(segment_t);
}

segment_t* segment_t::next()
{
    return reinterpret_cast<segment_t*>
    (
        reinterpret_cast<char*>(memory()) + size
    );
}

memory_manager_t::memory_manager_t(char* memory, std::size_t bytes)
    : __begin(reinterpret_cast<segment_t*>(memory))
    , __end(reinterpret_cast<segment_t*>(memory + bytes))
{
    assert(bytes > sizeof(segment_t));

    auto segment = new (begin()) segment_t;
    segment->size = bytes - sizeof(segment_t);
    segment->is_used = false;
}

void* memory_manager_t::add_segment(std::size_t size)
{
    for (auto segment = begin(); segment != end(); segment = segment->next())
    {
        if (segment->is_used)
        {
             continue;
        }

        if (segment->size >= sizeof(segment_t) + size)
        {
            auto diff = segment->size - size;

            segment->size = size;
            segment->is_used = true;

            auto created = new (segment->next()) segment_t;

            created->size = diff - sizeof(segment_t);
            created->is_used = false;
        }
        else if (segment->size == size)
        {
            segment->is_used = true;
        }
        else
        {
            continue;
        }

        return segment->memory();
    }
    return nullptr;
}

static void extend_segment(segment_t* extendable, segment_t const* segment)
{
    assert(extendable->next() == segment);

    extendable->size += sizeof(segment_t) + segment->size;
    extendable->is_used = false;
    segment->~segment_t();
}

bool memory_manager_t::remove_segment(void* address)
{
    auto lhs = end();
    for (auto segment = begin(); segment != end(); lhs = segment, segment = segment->next())
    {
        if (address != segment->memory())
        {
            continue;
        }
        
        auto rhs = segment->next();

        segment->is_used = false;

        if (rhs != end() && !rhs->is_used)
        {
            extend_segment(segment, rhs);
        }
        if (lhs != end() && !lhs->is_used)
        {
            extend_segment(lhs, segment);
        }

        return true;
    }
    return false;
}