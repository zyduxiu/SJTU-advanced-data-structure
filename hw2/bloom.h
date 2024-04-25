//
// Created by zhangyue on 2024/3/6.
//

#ifndef HW2_BLOOM_H
#define HW2_BLOOM_H
#include "MurmurHash3.h"
#include <iostream>
#include <bitset>
#include <vector>

const uint64_t m_size=500;
class Bloom{
private:
    uint64_t bitsetSize;
    std::bitset<m_size> m_bits={0};
    uint64_t numHashes;
    uint64_t m_hasher[2];
    uint64_t rand[2];
    uint64_t seed;
public:
    inline Bloom(uint64_t a,uint64_t b,uint64_t seed) : bitsetSize(a),numHashes(b),seed(seed) {};
    inline void insert(uint64_t &key){
        uint64_t pos;
        uint64_t tmp=key;
//        for(uint64_t i=0;i<numHashes;i++){
//            MurmurHash3_x64_128(&tmp,sizeof(tmp),seed,m_hasher);
//            tmp=m_hasher[1]%bitsetSize;
//            m_bits[tmp]=1;
//        }



//        for(uint64_t i=0;i<numHashes;i++){
//            MurmurHash3_x64_128(&tmp+i,sizeof(tmp+i),seed,m_hasher);
//            pos=m_hasher[1]%bitsetSize;
//            m_bits[pos]=1;
//        }


        for(uint64_t i=0;i<numHashes;i++) {
            uint64_t tt=(i+1237)*tmp;
            MurmurHash3_x64_128(&tt,sizeof(tt),seed,rand);
            uint64_t k=rand[0]%346;
            uint64_t g=tmp+k;
            MurmurHash3_x64_128(&g, sizeof(g), seed, m_hasher);
            pos=m_hasher[1]%bitsetSize;
            m_bits[pos]=true;
        }
    }
    inline std::bitset<m_size> getbits(){
        return m_bits;
    }
    inline bool check(uint64_t &key){
        uint64_t pos=0;
        uint64_t tmp=key;
//        for(uint64_t i=0;i<numHashes;i++){
//            MurmurHash3_x64_128(&tmp,sizeof(tmp),seed,m_hasher);
//            tmp=m_hasher[1]%bitsetSize;
//            if(m_bits[tmp]==false){
//                return false;
//            }
//        }
//        return true;



//        for(uint64_t i=0;i<numHashes;i++){
//            MurmurHash3_x64_128(&tmp+i,sizeof(tmp+i),seed,m_hasher);
//            pos=m_hasher[1]%bitsetSize;
//            if(m_bits[pos]==false){
//                return false;
//            }
//        }
//        return true;



        for(uint64_t i=0;i<numHashes;i++){
            uint64_t tt=(i+1237)*tmp;
            MurmurHash3_x64_128(&tt,sizeof(tt),seed,rand);
            uint64_t k=rand[0]%346;
            uint64_t g=tmp+k;
            MurmurHash3_x64_128(&g, sizeof(g), seed, m_hasher);
            pos=m_hasher[1]%bitsetSize;
            if(m_bits[pos]==false){
                return false;
            }
        }
        return true;
    }

};

#endif //HW2_BLOOM_H
