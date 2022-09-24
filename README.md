# LIRS_cache_algorithm - a program that implements an caching algorithm named LIRS

### Algorithm implemented according to its description in the paper: 
Song Jiang and Xiaodong Zhang. 2002. LIRS: An Efficient Low InterReference Recency Set Replacement Policy to Improve Buffer Cache
Performance. In Proceedings of the 2002 ACM SIGMETRICS International Conference on Measurement and Modeling of Computer Systems
(SIGMETRICS ’02). 31–42.
PDF article file link -> https://ranger.uta.edu/~sjiang/pubs/papers/jiang02_LIRS.pdf

### Installing and running

Prerequisites:
1) gtest should be installed on your system
2) cmake should be installed on your system

1. Create an empty folder where you want to build your project (e.g. it is called build)
2. `cd build`
3. run `cmake ..`
4. run `make` or `cmake --build`

After following this set of instructions all apps cna be found in `build/apps` in each separate directory. 
All the tests lie in a separate folder `build/tests`.