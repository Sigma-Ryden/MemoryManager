#include <cstddef> // size_t
#include <utility> // forward
#include <memory> // operator new

#include <string>
#include <map>
#include <iostream>

#include "TSmartAllocator.h"
#include "TMemoryManager.h"
#include "TSharedMemoryManager.h"

template <typename T>
using TSharedMemoryAllocator = TSmartAllocator<T, TSharedMemoryManager>;

void ShowMemoryStat() {
    std::cout << "\nMEMORY STAT:\n";

    auto segmentTable = TSharedMemoryManager::segment_table();

    auto dynSegment = segmentTable.dynamic_segment();
    std::cout << dynSegment->address() << ' ' << dynSegment->size() << ' ' << dynSegment->is_used() << "\n";
    //for(auto i = 0; i<dynSegment.Size(); ++i) std::cout << ((char*)dynSegment.Address())[i];
    std::cout << '\n';

    for(auto& segment:segmentTable) {
        std::cout << segment.address() << ' ' << segment.size() << ' ' << segment.is_used() << ": ";
        for(auto i = 0; i<segment.size(); ++i) std::cout << ((char*)segment.address())[i];
        std::cout << '\n';
    }
}

int main() {
    TSharedMemoryManager::init();
    {
        //#define TSharedMemoryAllocator std::allocator
        using shared_string = std::basic_string<char, std::char_traits<char>, TSharedMemoryAllocator<char>>;
        using shared_map = std::map<int, shared_string, std::less<>, TSharedMemoryAllocator<std::pair<const int, shared_string>>>;

        shared_map m;
        m[1] = "Hi";
        ShowMemoryStat();
        m[3] = "World";
        m.erase(1);
        m[2] = "Hello";
        m[5] = "0123456789qwertyuiopasdfghjklzxcvbnm";
        m.erase(5);
        m.erase(2);    ShowMemoryStat();
        for(auto& [k, v]:m) std::cout << k << ':' << v << '\n';

    ShowMemoryStat();
    }

    TSharedMemoryAllocator<int> alloc;
    auto pi = alloc.allocate(1);
    new (pi) int(123);

    ShowMemoryStat();

    return 0;
}
