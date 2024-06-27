#include <iostream>

#include "MemoryManager.h"

constexpr auto memory_size = 1000;
memory_manager_t memory( new char[memory_size], memory_size );

void ShowMemoryStat(const char* text)
{
    std::cout << std::boolalpha;
    std::cout << text << '\n';
    for(auto i = 0; i<32; ++i) std::cout << '-';
    std::cout << '\n';
    auto segmentTable = *memory.segment_table();
    for(auto& segment:segmentTable) {
        std::cout   << "address:\t" << segment.address
                    << "\nsize:\t" << segment.size
                    << "\nis_used:\t" << segment.is_used
                    << "\ndata:\t";
        //for(auto i = 0; i<segment.size; ++i) std::cout << ((char*)segment.address)[i];
        std::cout << "\n\n";
    }

    for(auto i = 0; i<32; ++i) std::cout << '-';
    std::cout << "\n\n";
}

#define DO(...) __VA_ARGS__; ShowMemoryStat(#__VA_ARGS__)

int main()
{
    DO(auto segment01 = memory.add_segment(1); new (segment01->address) char(1));
    DO(auto segment02 = memory.add_segment(2); new (segment02->address) short(2));
    DO(memory.remove_segment(segment01->address));
    DO(auto segment04 = memory.add_segment(4); new (segment04->address) int(4));
    DO(auto segment11 = memory.add_segment(1); new (segment11->address) char(1));
    DO(memory.remove_segment(segment02->address));

    return 0;
}
