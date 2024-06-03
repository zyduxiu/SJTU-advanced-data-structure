#include "kvstore.h"
#include <string>
#include  <fstream>
#include "skiplist.h"
#include "vlog.h"
#include <chrono>
#include <algorithm>
#include "utils.h"
#define mem_already_deleted "~![ERROR] ALREADY DELETED!~"
#define mem_not_exist "~![ERROR] MemTable No Exist!~"
#define delete_tag "~DELETED~"
#define sstvalue_noFound "~![ERROR] data not found!~"
KVStore::KVStore(const std::string &dir, const std::string &vlog) : KVStoreAPI(dir,vlog)
{
    t_dir=dir;
    t_vlog=vlog;
    Vlog newvlog(t_vlog);
    vlog1=newvlog;
    this->mmtable=new memtable;
    timestamp=0;
    utils::mkdir("./data/level-0");

}

//void KVStore::storevlog() {
//    std::ofstream file(t_vlog,std::ios::binary);
//    std::ofstream pfile(t_dir,std::ios::binary);
//    skiplist::node *p=list.head;
//    uint64_t index=0;
//    while(p!= nullptr){
//        std::string dtmp=p->data;
//        int ktmp=p->key;
//        int ltmp=p->data.size();
//        short checksum= 3;
//        file<<'m'<<checksum<<ktmp<<ltmp<<dtmp;
//        p=p->right[0];
//        index+=15+ltmp;
////        pfile<<
//    }
//    file.close();
//}

KVStore::~KVStore()
{
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */

SSTable* KVStore::loadtofile(uint64_t timestamp,std::string path) {
    skiplist::node *p=mmtable->list->head;
    p=p->right[0];
    while(p!= nullptr){
        vlog1.append(p->key,p->data);
        p=p->right[0];
    }
    std::vector<group> tmp=vlog1.addtofile();

    SSTable* tab=new SSTable(t_vlog,path,tmp,timestamp);
    return tab;
}

void KVStore::put(uint64_t key, const std::string &s)
{
    if(this->mmtable->checkwhetherfull(key,s)){
        this->mmtable->put(key,s);
        return;
    }
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::microseconds mstime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    this->timestamp++;
    std::string newFilePath = this->t_dir + "/level-0/" + std::to_string(mstime.count()) + ".sst";
    SSTable* curr=loadtofile(timestamp,newFilePath);
    this->saveCache[0][mstime.count()] = curr;

    this->mmtable->reset();
    //for compaction
    this->mmtable->put(key,s);
//    mmtable->put(key,s);
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	std::string val= this->mmtable->get(key);
    if(val!=mem_already_deleted&&val!=mem_not_exist){
        return val;
    }
    if(val==mem_already_deleted){
        return "";
    }
    if(val != mem_not_exist)
        return val;
    else{
        uint64_t Timestamp=0;
        bool ifFound=false;
        std::string validx;
        for(auto level=this->saveCache.begin();level!=this->saveCache.end();level++){
            for(auto sstable = level->second.rbegin(); sstable != level->second.rend(); sstable++) {
                SSTable *currentSSTable = sstable->second;
                if((key<currentSSTable->header.min||key>currentSSTable->header.max)
                ||!currentSSTable->bloomFilter->contains(key)
                   ){
                    continue;//bloomfilter;
                }
                if(currentSSTable->header.time>=Timestamp&&currentSSTable->getfromcache(key,vlog1)!= sstvalue_noFound ){
                     validx=currentSSTable->getfromcache(key,vlog1);
                        Timestamp=currentSSTable->header.time;
                        ifFound=true;
                }
            }
        }
        if(ifFound&&validx!=delete_tag){
            return validx;
        }
        if(!ifFound){
            return "";
        }
        if(ifFound&&validx==delete_tag){return  "";}
        else if(ifFound){
            return validx;
        }
    }
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	if(this->get(key)==""){
        return false;
    }
    else{
        this->put(key,delete_tag);
        return true;
    }
}

/* This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
    mmtable->reset();
    for(auto level = this->saveCache.begin(); level != this->saveCache.end(); level++){
        for(auto sstable = level->second.begin(); sstable != level->second.end(); sstable++){
            sstable->second->clear();
            delete sstable->second;
        }
    }
    this->saveCache.clear();
    this->vlog1.clear();
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list1)
{
    mmtable->scan(key1,key2,list1);
    std::vector<scanstruct> tmp;
    for(auto level = this->saveCache.begin(); level != this->saveCache.end(); level++){
        for(auto sstable = level->second.begin(); sstable != level->second.end(); sstable++){
            sstable->second->scan(key1,key2,tmp,vlog1);
        }
    }
    for (const auto& s : tmp) {
        bool found = false;
        for (auto it = list1.begin(); it != list1.end(); ++it) {
            if (it->first == s._key) {
                found = true;
                break;
            }
        }
        if (!found&&s._value!=delete_tag) {
            list1.emplace_back(s._key, s._value);
        }
    }
    std::vector<std::pair<uint64_t, std::string>> vec(list1.begin(), list1.end());
    std::sort(vec.begin(), vec.end(),
              [](const std::pair<uint64_t, std::string>& a, const std::pair<uint64_t, std::string>& b) {
                  return a.first < b.first;
              }
    );
    list1.clear();
    list1.insert(list1.begin(), vec.begin(), vec.end());

}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid value.
 * chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size)
{
}

