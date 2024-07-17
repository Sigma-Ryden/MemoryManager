#include <iostream>

#include "MemoryManager.h"

memory_manager_t& __memory_manager()
{
    constexpr auto bytes = 1000;
    static memory_manager_t manager(new char[bytes], bytes);
    return manager;
}

memory_manager_t& memory = __memory_manager();

void stats(char const* text)
{
    static const auto border_length = 32;

    std::cout << '\n';
    std::cout << std::boolalpha;
    std::cout << text << '\n';

    for (auto i = 0; i<border_length; ++i) std::cout << '-';
    std::cout << '\n';

    for (auto segment = memory.begin(); segment != memory.end(); segment = segment->next())
    {
        std::cout << "segment: " << segment
                  << "\nmemory: " << segment->memory()
                  << "\nsize: " << segment->size
                  << "\nis_used: " << segment->is_used
                  << "\n\n";
    }

    for (auto i = 0; i<border_length; ++i) std::cout << '-';
    std::cout << '\n';
}

#define DO(...) __VA_ARGS__; stats(#__VA_ARGS__)
#define DM(...) std::cout << #__VA_ARGS__ << ": " << static_cast<void*>(reinterpret_cast<char*>(__VA_ARGS__) - sizeof(segment_t)) << '\n'

void simple_test()
{
    //if (false)
    {
        DO((void)memory);
        DO(auto segment01 = memory.add_segment(1)); DM(segment01);
        DO(auto segment04 = memory.add_segment(4)); DM(segment04);
        DO(auto segment02 = memory.add_segment(2)); DM(segment02);
        
        DO(memory.remove_segment(segment01));
        DO(memory.remove_segment(segment02));
        DO(memory.remove_segment(segment04));
    }
    //else
    {
        DO((void)memory);
        DO(auto segment01 = memory.add_segment(1)); DM(segment01);
        DO(auto segment02 = memory.add_segment(2)); DM(segment02);
        DO(memory.remove_segment(segment01));
        DO(auto segment04 = memory.add_segment(4)); DM(segment04);
        DO(auto segment11 = memory.add_segment(1)); DM(segment11);
        DO(memory.remove_segment(segment02));
        DO(memory.remove_segment(segment11));
        DO(memory.remove_segment(segment04));
    }
}


#include <string>
#include <map>

#include "SegmentAllocator.hpp"

void stl_test()
{
    using string_allocator_t = segment_allocator_t<char>;
    using string_t = std::basic_string<char, std::char_traits<char>, string_allocator_t>;

    using map_allocator_t = segment_allocator_t<std::pair<string_t const, float>>;
    using map_t = std::map<string_t, float, std::less<>, map_allocator_t>;
    
    {
        string_t str;
        DO((void)str);
        DO(str = "01020304050607080910111213141516171819202122232425");
    }
    DO((void)"~str");

    {
        map_t map;
        DO((void)map);
        DO(map["Tom"] = 21.5);
        DO(map["Ren"] = 22.3);
        DO(map.erase("Tom"));
        DO(map["Jully"] = 32.2);
        DO(map["0123456789qwertyuiopasdfghjklzxcvbnm"]);
        DO(map.erase("Jully"));
    }
    DO((void)"~map");
}


int main()
{
    simple_test();
    stl_test();

    return 0;
}