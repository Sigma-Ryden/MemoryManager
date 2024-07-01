#include <iostream>

#include "MemoryManager.h"

constexpr auto memory_size = 1000;
memory_manager_t memory( new char[memory_size], memory_size );

void stats(char const* text)
{
    std::cout << std::boolalpha;
    std::cout << text << '\n';
    for(auto i = 0; i<32; ++i) std::cout << '-';
    std::cout << '\n';
    for (auto it = memory.begin(); it != memory.end(); it = it->next())
    {
        std::cout << "address: " << reinterpret_cast<segment_t*>(reinterpret_cast<char*>(it) + sizeof(segment_t))
                  << "\nsize: " << it->size
                  << "\nis_used: " << it->is_used
                  << "\n\n";
    }
    for(auto i = 0; i<32; ++i) std::cout << '-';
    std::cout << "\n\n";
}

#define DO(...) __VA_ARGS__; stats(#__VA_ARGS__)

int main()
{
    DO(((void)memory, 0));
    DO(auto segment01 = memory.add_segment(1));
    DO(auto segment04 = memory.add_segment(4));
    DO(auto segment02 = memory.add_segment(2));
    DO(memory.remove_segment(segment01));
    DO(memory.remove_segment(segment02));
    DO(memory.remove_segment(segment04));

    return 0;
}
