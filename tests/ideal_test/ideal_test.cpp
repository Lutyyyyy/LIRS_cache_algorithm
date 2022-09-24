#include <gtest/gtest.h>
#include "../../include/ideal_cache/ideal_cache.hpp"

TEST(Cachetest, test1) {
    std::vector<int> arr1 = {1, 2, 1, 2, 1, 2};
    EXPECT_EQ(4, ideal_cache::run_ideal_cache(2, 6, arr1));
}

TEST(CacheTest, test2) {
    std::vector<int> arr2 = {0, 1, 2, 3, 0, 1, 2, 3};
    EXPECT_EQ(4, ideal_cache::run_ideal_cache(4, 8, arr2));
}

TEST(CacheTest, test3) {
    std::vector<int> arr3 = {4, 7, 3, 6, 10, 11, 6, 8, 5, 3, 4, 7, 8, 6, 4, 4, 5, 6, 7, 8};
    EXPECT_EQ(11, ideal_cache::run_ideal_cache(5, 20, arr3));
}

TEST(CacheTest, test4) {
    std::vector<int> arr4 = {2, 2, 1, 3, 3, 4, 2, 1, 6, 5};
    EXPECT_EQ(4, ideal_cache::run_ideal_cache(3, 10, arr4));
}

TEST(CacheTest, test5) {
    std::vector<int> arr5 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(24, ideal_cache::run_ideal_cache(1, 25, arr5));
}

TEST(CacheTest, test6) {
    std::vector<int> arr6 = {2, 4, 2, 4, 6, 4, 6, 7, 6, 6, 7, 9, 6, 4, 3, 5, 7, 8, 6, 5, 4, 3, 4, 5, 7, 8, 8, 7, 6};
    EXPECT_EQ(19, ideal_cache::run_ideal_cache(5, 29, arr6));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}