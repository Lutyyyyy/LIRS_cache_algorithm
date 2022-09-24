#include <cassert>
#include <iostream>

#include "../../include/ideal_cache/ideal_cache.hpp"


int main() {

    int nInputs = 0;
    size_t cache_size = 0;

    std::cin >> cache_size >> nInputs;
    assert(std::cin.good());

    std::vector<int> Keys_vect = {};

    for (int i = 0; i < nInputs; i++)
    {
        int key;
        std::cin >> key;
        assert(std::cin.good());

        Keys_vect.push_back(key);
    }

    ideal_cache::ideal_cache_t<int> cch {cache_size};
    
    int hits = cch.count_ideal_cache_hits (nInputs, Keys_vect);
    std::cout << hits << std::endl;
    
    return 0;
}