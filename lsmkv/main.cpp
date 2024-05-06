//
// Created by zhangyue on 2024/4/5.
//
#include "kvstore.h"

int main() {

    KVStore newstore( "C:\\Users\\zhangyue\\Desktop\\","C:\\Users\\zhangyue\\Desktop\\vlog");
    for(int i=0;i<=512;i++){
        std::string str = std::to_string(i);
        std::string tmp="fuck"+str;
        newstore.put(i,tmp);
    }
//    newstore.mmtable->list->print();
    for(int i=0;i<=512;i++){
        std::string get=newstore.get(i);
        std::cout<<get;
    }
    newstore.put(1031,"fuck");
    std::string tmp21=newstore.get(102);
}