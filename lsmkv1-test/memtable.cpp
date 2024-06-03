#include "memtable.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <map>
#define delete_tag "~DELETED~"
#define mem_already_deleted "~![ERROR] ALREADY DELETED!~"
#define mem_not_exist "~![ERROR] MemTable No Exist!~"

memtable::memtable() {
    list=new skiplist::skiplist_type;
    Spacesize=0;
}
memtable::~memtable(){
    delete list;
    list= nullptr;
}

void memtable::put(uint64_t key, const std::string &s){
    std::string tryfind=list->get(key);
    if(tryfind!=""){
        this->list->put(key,s);
    }else{
        this->list->put(key,s);
        Spacesize+=1;
    }
}

bool memtable::del(uint64_t key){
    auto tryfind=this->list->get(key);
    if(tryfind==""){
        return false;
    }
    tryfind=delete_tag;
    Spacesize+=1;
    return true;
}

std::string memtable::get(uint64_t key){
    std::string tryfind=this->list->get(key);
    if(tryfind!=""){
        std::string val=tryfind;
        if(val==delete_tag)
            return mem_already_deleted;
        return val;
    }
    else{
        return mem_not_exist;
    }
}

void memtable::reset(){
    this->list->clear();
    Spacesize=0;
}

void memtable::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) {
    list=this->list->scan(key1,key2);
}

bool memtable::checkwhetherfull(uint64_t key, const std::string &s) {
    if(Spacesize>404){
        return false;
    }
    return true;
}
