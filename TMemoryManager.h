#ifndef TMEMORY_MANAGER_H
#define TMEMORY_MANAGER_H

#include <cstddef> // size_t
#include <utility> // pair

class TMemorySegment {
    public:
    TMemorySegment(void* address, std::size_t size);

    public:
    void* address();
    void address(void* address);

    public:
    std::size_t size();
    void size(std::size_t size);

    public:
    bool is_used();
    void used(bool yes);

    protected:
    // head address of memory segment
    void* address_;

    // last byte using for is_used state, other bytes for segment size
    std::size_t meta_;
};

class TMemorySegmentTable {
    public:
    TMemorySegmentTable(void* root, std::size_t capacity);

    public:
    TMemorySegment* add_segment(std::size_t size);
    bool merge_segment(TMemorySegment* segment);
    TMemorySegment* split_segment(TMemorySegment* segment, std::size_t size);

    public:
    TMemorySegment* find_segment(void* address);
    TMemorySegment* find_segment(std::size_t size);

    public:
    TMemorySegment* begin();
    TMemorySegment* end();

    public:
    TMemorySegment* dynamic_segment(); // temp

    protected:
    void* NewAllocationAddress(std::size_t size);

    protected:
    TMemorySegment* InsertSegment(TMemorySegment* position, std::size_t size);
    bool EraseSegment(TMemorySegment* position);
    std::pair<TMemorySegment*, TMemorySegment*> NeighbourSegments(TMemorySegment* segment);

    protected:
    void MergeSegmentImplementation(TMemorySegment* extendable, TMemorySegment* segment);

    protected:
    void* m_pRoot = nullptr;
    std::size_t m_uSize = 0;
};

class TMemoryManager {
    public:
    void memory(void* memory, std::size_t size);
    void init();

    public:
    bool remove_segment(void* address);
    TMemorySegment* add_segment(std::size_t size);

    public:
    TMemorySegmentTable* segment_table();

    protected:
    void* s_pMemory = nullptr;
    std::size_t s_uSize = 0;
};

#endif // TMEMORY_MANAGER_H
