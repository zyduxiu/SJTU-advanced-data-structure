
#ifndef LSMKV_MEMTABLE_H
#define LSMKV_MEMTABLE_H
#include "skiplist.h"
#include "utils.h"
#include <fstream>
#include <list>

class memtable{
private:

    size_t Spacesize;
public:
    skiplist::skiplist_type* list;
    memtable();
    ~memtable();
    bool checkwhetherfull(uint64_t key,const std::string &s);
    void put(uint64_t key, const std::string &s);

    std::string get(uint64_t key);

    bool del(uint64_t key);

    void reset();

    void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list);


};

#endif //LSMKV_MEMTABLE_H
