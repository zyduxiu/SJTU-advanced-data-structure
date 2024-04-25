#include <iostream>
#include "MurmurHash3.h"
#include "bloom.h"

const uint64_t m_hashes=5;
int main() {
    const uint64_t times=10000;
    int sum=0;
    for(uint64_t k=0;k<times;k++) {
        Bloom bloom(m_size, m_hashes,k);
        uint64_t cnt = 0;
        for (uint64_t i = 0; i < 100; i++) {
            bloom.insert(i);
        }

        for (uint64_t i = 100; i < 200; i++) {
            bool ss=bloom.check(i);
            if (ss == true) {
                bool tmp = bloom.check(i);
                cnt++;
            }
        }
        sum+=cnt;
    }
    double avg=(double((sum)/10000.0))/100.0;
    std::cout<<avg;
//    std::bitset<1000> a=bloom.getbits();
//    uint64_t key = 522123456789;
//    uint64_t hash[3] = {0};
//
//    MurmurHash3_x64_128(&key, sizeof(key), 2, hash);
//
//    std::cout << "Hash value: ";
//    for(int i = 0; i < 3; i++) {
//        std::cout << hash[i] << " ";
//    }
//    std::cout << std::endl;
//    //mod m-1
//    //only use hash[1]
//    int m = 100;
//    std::cout << hash[1]%(m-1) << " ";



    return 0;
}
