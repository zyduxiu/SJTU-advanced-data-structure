#ifndef LSMKV_DATASTRUCTURE_H
#define LSMKV_DATASTRUCTURE_H
#include <iostream>
#include <cstdint>
#include <vector>
#include "utils.h"
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
    Entry(uint64_t key, uint32_t vlen,std::string v): key(key),vlen(vlen), val(v) {
    	this->Magic=0xff;
    	this->key=key;
    	this->val=v;
    	this->vlen=v.size();
    	std::vector<unsigned char>data;
    	
    	unsigned char* keyPtr=reinterpret_cast<unsigned char*>(&this->key);
    	for(size_t i=0;i<sizeof(this->key);i++){
    	     data.push_back(keyPtr[i]);
    	}
    	unsigned char* vlenPtr=reinterpret_cast<unsigned char*>(&this->vlen);
    	for(size_t i=0;i<sizeof(this->vlen);i++){
    	     data.push_back(vlenPtr[i]);
    	}
    	for(const char& c:this->val){
    	     data.push_back(static_cast<unsigned char>(c));
    	}
    	
    	this->Checksum=utils::crc16(data);
    	
    }
};

struct gcc{
    uint8_t Magic=0xff;
    uint16_t Checksum;
    uint64_t key;
    uint32_t vlen;
    uint64_t offset;
    std::string value;
};

struct Range
{
    uint64_t min, max;
    Range(const uint64_t &i, const uint64_t &a): min(i), max(a) {}
};



#endif //LSMKV_DATASTRUCTURE_H
