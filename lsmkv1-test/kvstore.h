#pragma once
#include "skiplist.h"
#include "kvstore_api.h"
#include "memtable.h"
#include "SSTable.h"
#include <vector>
#include "utils.h"
#include "vlog.h"
#include <map>
#define delete_tag "~DELETED~"
class KVStore : public KVStoreAPI
{
	// You can add your implementation here
private:

    std::string t_dir,t_vlog;
    std::vector<std::vector<SSTable*>> cache;

public:
    memtable* mmtable;
    Vlog vlog1;
    uint64_t timestamp=0;
    std::map<uint64_t, std::map<uint64_t, SSTable*> > saveCache;
    std::string getdir(){return t_dir;}
	KVStore(const std::string &dir, const std::string &vlog);
    SSTable* loadtofile(uint64_t timestamp,std::string path);
	~KVStore();
    void storevlog();
	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;
	void seektail(Vlog* vlog);
	void reset() override;
    void sstcheck(std::string path);

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) override;

 uint64_t whichmerge();

    void Compaction(uint64_t level);
    
	void gc(uint64_t chunk_size) override;
};
