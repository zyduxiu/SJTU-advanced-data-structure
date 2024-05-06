#ifndef LSMKV_DATASTRUCTURE_H
#define LSMKV_DATASTRUCTURE_H
#include <iostream>
#include <cstdint>
struct Header{
    uint64_t time;
    uint64_t count;
    uint64_t min;
    uint64_t max;
};

struct group{
    uint64_t key;
    uint64_t offset;
    uint32_t vlen;
    group(uint64_t key = 0, uint64_t offset = 0,uint32_t vlen=0): key(key), offset(offset),vlen(vlen) {}
};

struct Entry
{
    uint8_t Magic=0xff;
    uint16_t Checksum;
    uint64_t key;
    uint32_t vlen;
    std::string val;
    Entry(short Checksum,uint64_t k, uint32_t vlen,std::string v): Checksum(Checksum),key(k),vlen(vlen), val(v) {}
};

struct Range
{
    uint64_t min, max;
    Range(const uint64_t &i, const uint64_t &a): min(i), max(a) {}
};



#endif //LSMKV_DATASTRUCTURE_H
