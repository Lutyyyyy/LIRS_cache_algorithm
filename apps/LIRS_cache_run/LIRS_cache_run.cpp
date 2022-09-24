#include <cassert>
#include <iostream>

#include "../../include/LIRS_cache/LIRS_cache.hpp"


int main () {
    
    int hits = 0; 
    int nInputs = 0;
    size_t cache_size = 0;

    std::cin >> cache_size >> nInputs;
    assert(std::cin.good());

    caches::cache_t<int> cch {cache_size};

    for (int i = 0; i < nInputs; i++)
    {
        int key;
        std::cin >> key;
        assert(std::cin.good());

        hits += cch.handle_new_page(key, slow_get_page_int);
    }

    std::cout << hits << std::endl;

    return 0;
}