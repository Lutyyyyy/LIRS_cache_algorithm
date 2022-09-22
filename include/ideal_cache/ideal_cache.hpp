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

int run_ideal_cache (size_t size, size_t nKeys, int* keys);


template <typename KeyT = int> struct ideal_cache_t {   
    
    std::unordered_map<KeyT, size_t> ideal_cache_map = {};
    size_t capacity;

    typedef struct distance_info_ {
        KeyT Key;
        int distance;
    } distance_info;


    ideal_cache_t (size_t cap) {
        capacity = cap;
    }


    distance_info* create_distance_array (size_t nKeys, KeyT* Keys) {
        
        distance_info* arr = new distance_info[nKeys];

        for (size_t i = 0; i < nKeys; i++) {
            auto hit = ideal_cache_map.find (Keys[i]);
            
            if (hit == ideal_cache_map.end ()) {
                arr[i].Key      = Keys[i];
                arr[i].distance = nKeys - i - 1;
                ideal_cache_map.insert({Keys[i], nKeys - 1 - i});
            }
            else {
                arr[i].Key = Keys[i];
                arr[i].distance = nKeys - i - 1;
                arr[nKeys - 1 - hit->second].distance = i + hit->second - nKeys + 1;
                hit->second = nKeys - i - 1;
            }
        }

        ideal_cache_map.clear();

        return arr;
    }


    int count_ideal_cache_hits (size_t nKeys, KeyT* keys) {

        distance_info* arr = create_distance_array (nKeys, keys);
        int hits = 0;

        for (size_t i = 0; i < nKeys; i++) {
            auto hit = ideal_cache_map.find (arr[i].Key);
            decrease_dist (ideal_cache_map);

            if (hit == ideal_cache_map.end ()) {
                if (ideal_cache_map.size() < capacity) {
                    ideal_cache_map.insert ({arr[i].Key, arr[i].distance});
                }

                else {
                    ideal_cache_map.erase (worst_dist_key(ideal_cache_map));
                    ideal_cache_map.insert ({arr[i].Key, arr[i].distance});
                }
            }

            else {
                hits++;
                hit->second = arr[i].distance;
            }
        }

        delete[] arr;
        return hits;
    }


    void print_dist_arr (size_t nKeys, KeyT* keys) {

        distance_info* arr = create_distance_array (nKeys, keys);
       
        for (size_t i = 0; i < nKeys; i++) 
            std::cout << arr[i].Key << ' ';
        
        std::cout << std::endl;

        for (size_t i = 0; i < nKeys; i++)
            std::cout << arr[i].distance << ' ';
        
        std::cout << std::endl;


        delete[] arr;
        return;
    }


    void decrease_dist (std::unordered_map<KeyT, size_t>& map) {
        
        for (auto n : map) n.second--;

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


int run_ideal_cache (size_t cache_size, size_t nKeys, int* keys) {
   
    ideal_cache_t<int> cache {cache_size};

    return cache.count_ideal_cache_hits(nKeys, keys);
}


}
