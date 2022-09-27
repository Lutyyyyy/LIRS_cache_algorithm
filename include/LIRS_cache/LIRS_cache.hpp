#pragma once

#include <stdio.h>
#include <iostream>
#include <cassert>
#include <iterator>
#include <list>
#include <vector>
#include <unordered_map>
#include <algorithm>


int slow_get_page_int (int key);


namespace caches {

int test_cache (size_t cache_size, int nKeys, std::vector<int>& arr);

template <typename Page_t, typename KeyT = int> struct cache_t {
    
    const int LIR = 1, HIR = 2;
    const int ERROR = -1, FULL = -2;
    const float HIR_blocks_percentage_coeff = 0.1;

    using T         = typename std::pair <KeyT, int>;
    using List_Iter = typename std::list<T>::iterator;


//Status and info structures==========================================
    
    struct Page_status_and_iterators {
        
        Page_t page;
        int status;
        List_Iter S_list_iter;
        List_Iter HIR_list_iter;

        Page_status_and_iterators (Page_t page_, int status_, List_Iter S_list_iter_, List_Iter HIR_list_iter_) {
            page = page_;
            status = status_;
            S_list_iter = S_list_iter_;
            HIR_list_iter = HIR_list_iter_;
        }
    };

//====================================================================


//structures and typenames for LIRS cache algorithm===================

    std::unordered_map<KeyT, Page_status_and_iterators*> hash_map = {};
    using Cache_Iter = typename std::unordered_map <KeyT, Page_status_and_iterators*>::iterator;

    std::unordered_map<KeyT, List_Iter> non_resident_HIR_hash_map = {};
    
    std::list<T> S_list = {};
    std::list<T> HIR_list = {};

//====================================================================


    size_t size = 0;
    size_t LIR_size = 0;
    size_t HIR_size = 0;

    cache_t (size_t sz) {
        
        assert (sz > 0);
        size = sz;
        
        if (size == 1)
            HIR_size = 0;
        else {
            HIR_size = (size_t) (size * HIR_blocks_percentage_coeff);       
            if (HIR_size == 0) 
                HIR_size = 1;
        }

        LIR_size = size - HIR_size;
    }

    ~cache_t () {

        for (auto Iter = hash_map.begin(), Iter_end = hash_map.end(); Iter != Iter_end; ++Iter)
            delete Iter->second;
    }

//===================================================================

    int fill_cache (KeyT key, Page_t (*slow_get_page) (KeyT key)) {
        
        if (size == 1) {
            if (!LIR_full() && !LIR_overflow()) {
                S_list.push_front ({key, LIR});
                hash_map.insert ({key, construct_page_structure (key, LIR, S_list.begin(), HIR_list.end(), slow_get_page)});

                return false;
            }

            return FULL;
        }

        auto hit = hash_map.find (key);

        if (hit == hash_map.end ()) {
            
            if (!LIR_full() && !LIR_overflow()) {
                S_list.push_front ({key, LIR});
                hash_map.insert ({key, construct_page_structure (key, LIR, S_list.begin(), HIR_list.end(), slow_get_page)});
            }
            else if (!HIR_full() && !HIR_overflow()) {
                HIR_list.push_front ({key, HIR});
                S_list.push_front ({key, HIR});
                hash_map.insert ({key, construct_page_structure (key, HIR, S_list.begin(), HIR_list.begin(), slow_get_page)});
            }
            else return FULL; 

            return false;
        }

        else { /* libo lir to lir libo hir to lir and botttom to hir*/
            if (hit->second->status == LIR) { 
                auto term_iter = S_list_move_front (hit);
                if (term_iter == S_list.end()) 
                    return ERROR;
                
                else {
                    hit->second->S_list_iter = term_iter;
                    return true;
                }
            }

            else {
                if (hit->second->S_list_iter != S_list.end()) {
                    S_list.erase(hit->second->S_list_iter);
                    hit->second->S_list_iter = S_list.end();
                }

                if (hit->second->HIR_list_iter != HIR_list.end()) {   
                    HIR_list.erase(hit->second->HIR_list_iter);
                    hit->second->HIR_list_iter = HIR_list.end();
                }

                hit->second->status = LIR;

                S_list.push_front ({key, LIR});
                hit->second->S_list_iter = S_list.begin();
                
                auto bottom = hash_map.find (S_list.back().first);
                bottom->second->status = HIR;
                move_bottom_LIR_to_HIR ();
            }

            return true;
        }
        
        return ERROR;
    }


    int update_cache (KeyT key, Page_t (*slow_get_page) (KeyT key)) {
        
        auto hit = hash_map.find (key);

        if (size == 1) {
            if (hit == hash_map.end()) {
                auto term = hash_map.find(S_list.front().first);
                if (term != hash_map.end()) {
                    delete term->second;
                    hash_map.erase(term);
                }
                S_list.clear();

                S_list.push_front ({key, LIR});
                hash_map.insert ({key, construct_page_structure (key, LIR, S_list.begin(), HIR_list.end(), slow_get_page)});
                
                return false;
            }

            return true;
        }
        
        if (hit == hash_map.end()) {  
            
            auto nonres_HIR_It = non_resident_HIR_hash_map.find (key);
            
            if (nonres_HIR_It == non_resident_HIR_hash_map.end()) {                            /*new*/        
                S_list.push_front ({key, HIR});
                HIR_list.push_front ({key, HIR});
            
                hash_map.insert ({key, construct_page_structure (key, HIR, S_list.begin(), HIR_list.begin(), slow_get_page)});
                delete_bottom_HIR_block (); 
            }

            else {
                S_list.erase (nonres_HIR_It->second);
                non_resident_HIR_hash_map.erase (nonres_HIR_It->first);

                S_list.push_front ({key, LIR});
                hash_map.insert ({key, construct_page_structure (key, LIR, S_list.begin(), HIR_list.end(), slow_get_page)});
            
                move_bottom_LIR_to_HIR ();
                if (HIR_overflow())
                    delete_bottom_HIR_block (); 
            }
  
            return false;  
        }

        else {
            if (hit->second->status == HIR) { 
                if (hit->second->S_list_iter == S_list.end()) {
                    auto term_iter = HIR_list_move_front (hit);
                    if (term_iter == HIR_list.end())
                        return ERROR;

                    hit->second->HIR_list_iter = term_iter;
                    S_list.push_front ({key, HIR});   
                    hit->second->S_list_iter = S_list.begin();            
                }

                else {                                     
                    HIR_list.erase (hit->second->HIR_list_iter);
                    hit->second->HIR_list_iter = HIR_list.end();
                    
                    S_list.erase (hit->second->S_list_iter);
                    hit->second->status = LIR;
                    S_list.push_front ({key, LIR});
                    hit->second->S_list_iter = S_list.begin();

                    move_bottom_LIR_to_HIR ();
                    if (HIR_overflow()) 
                        delete_bottom_HIR_block (); 
                }
            }

            else {
                auto term_iter = S_list_move_front (hit);
                if (term_iter == S_list.end()) 
                    S_list.push_front ({key, LIR});
                
                hit->second->S_list_iter = S_list.begin();
            }

            return true;
        }
    
        return ERROR;
    }


    int handle_new_page (KeyT key, Page_t (*slow_get_page) (KeyT key)) {
        
        if (hash_map.size() < size)
            return fill_cache(key, slow_get_page);

        return update_cache(key, slow_get_page);
    }

    
    Page_status_and_iterators* construct_page_structure (KeyT key, int status, List_Iter S_list_It, List_Iter HIR_list_It, Page_t (*slow_get_page)(KeyT key)) {
        
        Page_status_and_iterators* Iterator = new Page_status_and_iterators (slow_get_page (key), status, S_list_It, HIR_list_It);

        return Iterator;
    }


    List_Iter S_list_move_front (Cache_Iter Iterator) {
        
        if (Iterator->second->S_list_iter == S_list.begin()) 
            return S_list.begin();
        
        else if (Iterator->second->S_list_iter != S_list.end()) {

            KeyT k = Iterator->first;
            int st = Iterator->second->status;
            
            S_list.erase (Iterator->second->S_list_iter);
            S_list_pruning (); 

            S_list.push_front({k, st});
            Iterator->second->S_list_iter = S_list.begin();
        }

        else 
            return S_list.end();
        

        return S_list.begin();
    }


    List_Iter HIR_list_move_front (Cache_Iter Iterator) {

        if (Iterator->second->HIR_list_iter == HIR_list.begin()) 
            return HIR_list.begin();
    
        else if (Iterator->second->HIR_list_iter != HIR_list.end()) {

            KeyT k = Iterator->first;
            int st = Iterator->second->status;
            
            HIR_list.erase (Iterator->second->HIR_list_iter); 

            HIR_list.push_front({k, st});
            Iterator->second->HIR_list_iter = HIR_list.begin();
        }

        else 
            return HIR_list.end();
        

        return HIR_list.begin();

    }


    void move_bottom_LIR_to_HIR () {
        
        HIR_list.push_front ({S_list.back().first, HIR});

        auto bottom = hash_map.find (S_list.back().first);

        if (bottom != hash_map.end()) {
            bottom->second->status = HIR;
            bottom->second->S_list_iter = S_list.end();        
            bottom->second->HIR_list_iter = HIR_list.begin();
        }

        S_list.pop_back();
        S_list_pruning();

        return;  
    }


    void S_list_pruning () {

        while (S_list.back().second == HIR) {
            auto bottom = hash_map.find (S_list.back().first);
            if (bottom != hash_map.end() && bottom->second->status == HIR)
                bottom->second->S_list_iter = S_list.end();

            auto hit = non_resident_HIR_hash_map.find (S_list.back().first);
            if (hit != non_resident_HIR_hash_map.end()) 
                non_resident_HIR_hash_map.erase(hit->first);

            S_list.pop_back();
        }

        return;
    }


    void delete_bottom_HIR_block () {
        
        auto non_res_HIR = hash_map.find (HIR_list.back().first);
        if (non_res_HIR != hash_map.end()) {
            if (non_res_HIR->second->S_list_iter != S_list.end())
                non_resident_HIR_hash_map.insert ({non_res_HIR->first, non_res_HIR->second->S_list_iter});
        }

        else return;
        

        delete non_res_HIR->second;
        hash_map.erase (HIR_list.back().first);

        HIR_list.pop_back();
        return; 
    }


    bool LIR_full()     const { return (  S_list.size() == LIR_size); }
    bool HIR_full()     const { return (HIR_list.size() == HIR_size); }
    bool LIR_overflow() const { return (  S_list.size() >  LIR_size); }
    bool HIR_overflow() const { return (HIR_list.size() >  HIR_size); }


//functions used to debug============================================
    void print_cache () {
        
        printf ("====================\n");
        for (auto const m : S_list) {
            if (m.second == LIR)
                printf (" %3d   LIR\n", m.first);
            else printf ("(%3d)  HIR\n", m.first);
        }

        printf ("\n\n");

        for (auto const n : HIR_list) 
            if (n.second == HIR)
                printf (" %3d   HIR\n", n.first);
            else printf (" %3d   LIR    ERROR\n", n.first);

        printf ("====================\n");

        return;
    }


    int run_cache (int nKeys, std::vector<int>& key_vect, Page_t (*slow_get_page) (KeyT key)) {
        
        int i = 0, hits = 0, res = 0;
        
        while (i < nKeys) {
            hits += res;
            res = fill_cache (key_vect[i++], slow_get_page);
            
            if (res == FULL)
                break;
            else if (res == ERROR) 
                return ERROR;
        }

        if (res >= 0) 
            hits += res;

        if (i == nKeys) 
            return hits;
        
        i -= 1;

        while (i < nKeys) {
            res = update_cache (key_vect[i++], slow_get_page);

            if (res == ERROR)
                return ERROR;
            
            hits += res; 
        }
            
        return hits;
    }

};


int test_cache (size_t cache_size, int nKeys, std::vector<int>& key_vect) {

    cache_t<int> cache {cache_size};
    
    return cache.run_cache (nKeys, key_vect, slow_get_page_int);
}

}


int slow_get_page_int (int key) { return key; }
