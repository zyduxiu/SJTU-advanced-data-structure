#include "memtable.h"
#include <cstddef>
#include <cstdint>
#include <string>
#define delete_tag "~DELETED~"
#define mem_already_deleted "~![ERROR] ALREADY DELETED!~"
#define mem_not_exist "~![ERROR] MemTable No Exist!~"

memtable::memtable() {
    list=new skiplist::skiplist_type;
    Spacesize=8192+32;
}
memtable::~memtable(){
    delete list;
    list= nullptr;
}

void memtable::put(uint64_t key, const std::string &s){
    std::string tryfind=list->get(key);
    if(tryfind!=""){
        this->list->put(key,s);
        if(s.size()>tryfind.size()){
            Spacesize+=s.size()-tryfind.size();
        }
        else{
            Spacesize-=tryfind.size()-s.size();
        }
    }else{
        this->list->put(key,s);
        Spacesize+=s.size()+12;
    }
}

bool memtable::del(uint64_t key){
    auto tryfind=this->list->get(key);
    if(tryfind==""){
        return false;
    }
    tryfind=delete_tag;
    if(sizeof(delete_tag)>tryfind.size()){
        Spacesize+=sizeof(delete_tag)-tryfind.size();
    }
    else{
        Spacesize-=tryfind.size()-sizeof (delete_tag);
    }
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
    Spacesize=8192+32;
}

void memtable::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) {
    list=this->list->scan(key1,key2);
}

bool memtable::checkwhetherfull(uint64_t key, const std::string &s) {
    size_t newSize = 12 + s.size();

    std::string tryFind = list->get(key);

    if (tryFind == "") {
        return (Spacesize + newSize <= 16384);
    } else {
        if (newSize > tryFind.size()) {
            return (Spacesize + newSize - tryFind.size() <= 16384);
        } else {
            return true;
        }
    }
}