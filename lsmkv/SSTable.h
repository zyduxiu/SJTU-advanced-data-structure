#ifndef LSMKV_SSTABLE_H
#define LSMKV_SSTABLE_H
#include "datastructure.h"
#include "Bloomfilter.h"
#include "vlog.h"
#include <vector>
#include <list>
#include "utils.h"

#define MAX_TABLE_SIZE 2097152

class SSTable{
public:
    std::string vlog_path;
    std::string path;
    Header header;
    BloomFilter* bloomFilter;
    std::vector<group> values;
    void loadfile(std::string dir);
    SSTable():bloomFilter(new BloomFilter()){} ;
    SSTable(std::string vlog_path,std::string path,std::vector<group> values,uint64_t time);
//    ~SSTable(){delete bloomFilter;};
    void savefile();
    void save(const std::string &dir);
    std::string getfromcache(uint64_t key,Vlog vlog);
//    void clear(){utils::rmfile(this->path.c_str());}
    void add(const Entry &entry);

};

class SSTablecache{
public:
    std::vector<SSTable* >cache;
};


#endif //LSMKV_SSTABLE_H
