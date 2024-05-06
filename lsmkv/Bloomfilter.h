
#ifndef LSMKV_BLOOMFILTER_H
#define LSMKV_BLOOMFILTER_H

#define FILTER_SIZE 81920
#include <cstdint>
#include <bitset>


class BloomFilter
{
private:

public:
    std::bitset<FILTER_SIZE> bitSet;
    BloomFilter() {bitSet.reset();}
    BloomFilter(char *buf);
    void add(const uint64_t &key);
    bool contains(const uint64_t &key);
    void save2Buffer(char* buf);

    std::bitset<FILTER_SIZE> *getSet() {return &bitSet;}
};

#endif //LSMKV_BLOOMFILTER_H
