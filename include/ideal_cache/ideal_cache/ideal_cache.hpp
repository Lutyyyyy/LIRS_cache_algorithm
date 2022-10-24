#pragma once

#include <iostream>
#include <cassert>
#include <iterator>
#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>


namespace ideal_cache {

int run_ideal_cache (size_t size, size_t nKeys, std::vector<int>& Keys);


template <typename KeyT = int> struct ideal_cache_t {   
    
    std::unordered_map<KeyT, size_t> ideal_cache_map = {};
    size_t capacity;

    using T = typename std::pair <KeyT, size_t>;
    std::vector<T> dist_vector;


    ideal_cache_t (size_t cap) : capacity(cap) {}


    void fill_distance_vector (size_t nKeys, std::vector<KeyT>& Keys) {

        for (size_t i = 0; i < nKeys; i++) {
            auto hit = ideal_cache_map.find (Keys[i]);
            
            if (hit == ideal_cache_map.end ()) {
                dist_vector.push_back({Keys[i], nKeys - i - 1});
                ideal_cache_map.insert({Keys[i], nKeys - 1 - i});
            }
            else {
                dist_vector.push_back({Keys[i], nKeys - i - 1});
                dist_vector[nKeys - 1 - hit->second].second = i + hit->second - nKeys + 1;
                hit->second = nKeys - i - 1;
            }
        }

        ideal_cache_map.clear();

        return;
    }


    int count_ideal_cache_hits (size_t nKeys, std::vector<KeyT>& Keys) {

        fill_distance_vector (nKeys, Keys);
        int hits = 0;

        for (size_t i = 0; i < nKeys; i++) {
            auto hit = ideal_cache_map.find (dist_vector[i].first);
            decrease_dist (ideal_cache_map);

            if (hit == ideal_cache_map.end ()) {
                if (ideal_cache_map.size() < capacity) {
                    ideal_cache_map.emplace (dist_vector[i]);
                }

                else {
                    auto worst = ideal_cache_map.find (worst_dist_key(ideal_cache_map));
                    if (worst != ideal_cache_map.end() && dist_vector[i].second < worst->second) {
                            ideal_cache_map.erase (worst_dist_key(ideal_cache_map));
                            ideal_cache_map.emplace (dist_vector[i]);
                    }
                }
            }

            else {
                hits++;
                hit->second = dist_vector[i].second;
            }
        }

        return hits;
    }


    void print_dist_arr (size_t nKeys, std::vector<KeyT>& Keys) {

        fill_distance_vector (nKeys, Keys);
       
        for (size_t i = 0; i < nKeys; i++) 
            std::cout << dist_vector[i].first << ' ';
        
        std::cout << std::endl;

        for (size_t i = 0; i < nKeys; i++)
            std::cout << dist_vector[i].second << ' ';
        
        std::cout << std::endl;

        return;
    }


    void decrease_dist (std::unordered_map<KeyT, size_t>& map) {
        
        for (auto& n : map) n.second--;

        return;
    }


    KeyT worst_dist_key (std::unordered_map<KeyT, size_t>& map) {

        int worst = 0;
        KeyT res = 0;
        
        for (auto const n : map) {
            if (n.second > worst) {
                worst = n.second;
                res = n.first;
            }
        } 

        return res;
    }

};


int run_ideal_cache (size_t cache_size, size_t nKeys, std::vector<int>& Keys) {
   
    ideal_cache_t<int> cache {cache_size};

    return cache.count_ideal_cache_hits(nKeys, Keys);
}


}

