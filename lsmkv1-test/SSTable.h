#ifndef LSMKV_SSTABLE_H
#define LSMKV_SSTABLE_H
#include "datastructure.h"
#include "Bloomfilter.h"
#include "vlog.h"
#include <vector>
#include <list>
#include "utils.h"
#include <map>
#define delete_tag "~DELETED~"
#define MAX_TABLE_SIZE 2097152
struct scanstruct{
    uint64_t time;
    uint64_t _key;
    std::string _value;
    scanstruct(){};
};
class SSTable{
public:
    std::string vlog_path;
    std::string path;
    Header header;
    BloomFilter* bloomFilter;
    std::vector<group> values;
    SSTable(std::string dir);
    SSTable():bloomFilter(new BloomFilter()){} ;
    long findindex(uint64_t key,std::vector<scanstruct> scanlist);
    void scan(uint64_t key1,uint64_t key2,std::map<uint64_t, std::map<uint32_t,std::string>>& scanlist,Vlog vlog);
    SSTable(std::string vlog_path,std::string path,std::vector<group> values,uint64_t time);
//    ~SSTable(){delete bloomFilter;};
    void savefile();
    void save(const std::string &dir);
    std::string getfromcache(uint64_t key,Vlog* vlog);
    void clear(){utils::rmfile(this->path.c_str());}
    void add(const Entry &entry);

};

class SSTablecache{
public:
    std::vector<SSTable* >cache;
};


#endif //LSMKV_SSTABLE_H
