#include <cassert>
#include <iostream>

#include "../../include/ideal_cache/ideal_cache.hpp"
#include "../../include/LIRS_cache/LIRS_cache.hpp"


int main() {

    int nInputs = 0;
    size_t cache_size = 0;

    std::cin >> cache_size >> nInputs;
    assert(std::cin.good());

    int LIRS_hits = 0;
    caches::cache_t<int> cch {cache_size};

    int* Keys_arr = new int[nInputs];

    for (int i = 0; i < nInputs; i++)
    {
        int key;
        std::cin >> key;
        assert(std::cin.good());

        Keys_arr[i] = key;
        
        LIRS_hits += cch.handle_new_page(key, slow_get_page_int);
    }

    ideal_cache::ideal_cache_t<int> ideal_cch {cache_size};
    int ideal_hits = ideal_cch.count_ideal_cache_hits (nInputs, Keys_arr);
    
    std::cout << "LIRS  hits: " << LIRS_hits << std::endl;
    std::cout << "ideal hits: " << ideal_hits << std::endl;
    
    return 0;
}