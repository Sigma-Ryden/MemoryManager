#include <new>

#include "TMemoryManager.h"

static std::size_t segment_meta(std::size_t size, bool state) {
    return (size << 1) + state;
}

TMemorySegment::TMemorySegment(void* address, std::size_t size) {
    address_ = address;
    meta_ = segment_meta(size, true);
}

void* TMemorySegment::address() { return address_; }
void TMemorySegment::address(void* address) { address_ = address; }

std::size_t TMemorySegment::size() { return meta_ >> 1; }
void TMemorySegment::size(std::size_t size) { meta_ = segment_meta(size, is_used()); }

bool TMemorySegment::is_used() { return meta_ & 1; }
void TMemorySegment::used(bool yes) { meta_ = segment_meta(size(), yes); }

TMemorySegmentTable::TMemorySegmentTable(void* head, std::size_t capacity) {
    m_pRoot = head;
    new (m_pRoot) TMemorySegment(reinterpret_cast<char*>(m_pRoot) + capacity, capacity);
}

TMemorySegment* TMemorySegmentTable::add_segment(std::size_t size) {
    if (size > dynamic_segment()->size()) return nullptr;

    auto address = NewAllocationAddress(size);
    auto segment = new (end()) TMemorySegment(address, size);
    ++m_uSize;

    return reinterpret_cast<TMemorySegment*>(segment);
}

bool TMemorySegmentTable::merge_segment(TMemorySegment* segment) {
    auto success = false;
    if (segment->is_used()) return false;

    auto [lhs, rhs] = NeighbourSegments(segment);

    if (lhs != nullptr && !lhs->is_used())
    {
        segment->address(lhs->address());
        MergeSegmentImplementation(segment, lhs);
        success = true;
    }

    if (rhs != nullptr && !rhs->is_used())
    {
        MergeSegmentImplementation(segment, rhs);
        success = true;
    }

    return success;
}

TMemorySegment* TMemorySegmentTable::split_segment(TMemorySegment* segment, std::size_t size) {
    TMemorySegment* inserted = nullptr;
    if (size < segment->size())
    {
        auto diff = segment->size() - size;
        segment->size(size);

        inserted = InsertSegment(segment, diff);
        if (inserted) inserted->used(false);
    }

    return inserted;
}

template <typename FilterType>
TMemorySegment* FindSegmentImplementation(TMemorySegment* first, TMemorySegment* last, FilterType filter) {
    for(auto it = first; it!=last; ++it) if( filter(*it) ) return it;
    return nullptr;
}

TMemorySegment* TMemorySegmentTable::find_segment(void* address) {
    return FindSegmentImplementation(begin(), end(), [address](auto segment) {
        return segment.address() == address;
    });
}

TMemorySegment* TMemorySegmentTable::find_segment(std::size_t size) {
    return FindSegmentImplementation(begin(), end(), [size](auto segment) {
        return !segment.is_used() && size <= segment.size();
    });
}

TMemorySegment* TMemorySegmentTable::begin() {
    return dynamic_segment() + 1;
}

TMemorySegment* TMemorySegmentTable::end() {
    return begin() + m_uSize;
}

TMemorySegment* TMemorySegmentTable::dynamic_segment() {
    return reinterpret_cast<TMemorySegment*>(m_pRoot);
}

// rewrite!
// extend dynamic segment after merging
void* TMemorySegmentTable::NewAllocationAddress(std::size_t size) {
    auto diff = dynamic_segment()->size()-size;

    dynamic_segment()->size(diff);
    return reinterpret_cast<char*>(m_pRoot)+diff;
}

// a [b] c d -> a [b] c d [x] -> a [b] c [x] d -> a [b] [x] c d
TMemorySegment* TMemorySegmentTable::InsertSegment(TMemorySegment* position, std::size_t size) {
    for(auto it = begin(); it!=end(); ++it) if(it==position) {
        auto newSegment = add_segment(size);
        for(auto lhs = end()-2, rhs = end()-1; lhs!=position; --lhs, --rhs) std::swap(*rhs, *lhs);
        return newSegment;
    }
    return nullptr;
}

// a [b] c d -> a c [b] d -> a c d [b]
bool TMemorySegmentTable::EraseSegment(TMemorySegment* position) {
    for(auto it = begin(); it!=end(); ++it) if(it==position) {
        for(auto lhs = position, rhs=position+1; rhs!=end(); ++lhs, ++rhs) std::swap(*lhs, *rhs);
        --m_uSize;
        return true;
    }
    return false;
}

std::pair<TMemorySegment*, TMemorySegment*> TMemorySegmentTable::NeighbourSegments(TMemorySegment* segment) {
    TMemorySegment* lhs = nullptr;
    TMemorySegment* rhs = nullptr;
    for(auto it = begin(); it!=end(); ++it) if(it==segment) {
        if( it>begin() ) lhs = it;
        if( it+1<end() ) rhs = it+1;
        return {lhs, rhs};
    }
    return {lhs, rhs};
}

void TMemorySegmentTable::MergeSegmentImplementation(TMemorySegment* extendable, TMemorySegment* segment) {
    extendable->size(extendable->size() + segment->size());
    EraseSegment(segment);
}

void TMemoryManager::memory(void* memory, std::size_t size) {
    s_pMemory = memory;
    s_uSize = size;
}

void TMemoryManager::init() {
    auto dynamicSegmentSpace = reinterpret_cast<char*>(s_pMemory) + sizeof(TMemorySegmentTable);
    auto dynamicSegmentCapacity = s_uSize - sizeof(TMemorySegmentTable);

    new (s_pMemory) TMemorySegmentTable(dynamicSegmentSpace, dynamicSegmentCapacity);
}

bool TMemoryManager::remove_segment(void* address) {
    auto segment = segment_table()->find_segment(address);
    if (segment)
    {
        segment->used(false);
        segment_table()->merge_segment(segment);
    }

    return segment != nullptr;
}

TMemorySegment* TMemoryManager::add_segment(std::size_t size) {
    auto segment = segment_table()->find_segment(size);
    if (segment)
    {
        segment_table()->split_segment(segment, size);
        segment->used(true);
    }
    else
    {
        segment = segment_table()->add_segment(size);
    }

    return segment;
}

TMemorySegmentTable* TMemoryManager::segment_table() {
    return reinterpret_cast<TMemorySegmentTable*>(s_pMemory);
}
