
// Created by zhangyue on 2024/4/8.
//
#include "SSTable.h"
#include  <fstream>
#include <map>
#define sstvalue_noFound "~![ERROR] data not found!~"
#define delete_tag "~DELETED~"
SSTable::SSTable(std::string dir) {
    std::ifstream file(dir, std::ios::binary);
    if(!file) {
        printf("Fail to open file %s", dir.c_str());
        exit(-1);
    }
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0,std::ios::beg);
    file.read((char*)&(this->header.time), 8);
    file.read((char*)&(this->header.count), 8);
    uint64_t length = header.count;
    file.read((char*)&(this->header.min), 8);
    file.read((char*)&(this->header.max), 8);
    this->bloomFilter = new BloomFilter(); 
   file.seekg( 8000,std::ios::cur);
    char *indexBuf = new char[length * 20];
    file.read(indexBuf, length * 20);
    for(uint32_t i = 0; i < length; ++i) {
        this->values.push_back(group(*(uint64_t*)(indexBuf + 20*i), *(uint64_t*)(indexBuf + 20*i + 8),*(uint32_t*)(indexBuf+20*i+16)));
    }
    for(int i=0;i<values.size();i++){
        bloomFilter->add(values[i].key);
    }

//    delete[] filterBuf;
    delete[] indexBuf;
    file.close();
}



std::string SSTable::getfromcache(uint64_t key,Vlog* vlog){
    int left = 0;
    int right = values.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (values[mid].key == key) {
            if(values[mid].vlen!=0) {
                return vlog->getval(values[mid].offset, values[mid].vlen);
            }
            else{
                return "";
            }
        }
        if (values[mid].key < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return sstvalue_noFound;
}


SSTable::SSTable(std::string vlog_path,std::string dir,std::vector<group> values,uint64_t timestamp) {
    this->vlog_path=vlog_path;
    this->path=dir;
    uint64_t len=values.size();
    uint64_t min_key=values[0].key;
    uint64_t max_key=values[len-1].key;
    Header tmp;tmp.count=len;tmp.min=min_key;tmp.max=max_key;tmp.time=timestamp;
    bloomFilter=new BloomFilter();
    for(int i=0;i<values.size();i++){
        bloomFilter->add(values[i].key);
    }
    this->header=tmp;
    this->values=values;
    savefile();
}
void SSTable::scan(uint64_t key1, uint64_t key2,std::map<uint64_t ,std::map<uint32_t,std::string>>  &scanlist,Vlog vlog) {
    if(key1>header.max||key2<header.min){
        return;
    }
    int i=0;
    while(values[i].key<key1){
        i++;
    }
    while(i!=values.size()&&values[i].key<=key2){
//        scanstruct tmpp;
//        tmpp._key=values[i].key;
//        tmpp._value=vlog.getval(values[i].offset,values[i].vlen);
//        tmpp.time=this->header.time;
//        uint64_t tmppp;
//        if(!scanlist.empty()) {
//            tmppp = scanlist[scanlist.size()-1]._key;
//        }
//        if(scanlist.empty()||tmppp<values[i].key){
//            scanlist.push_back(tmpp);
//        }
//        else {
//            long tmp = findindex(values[i].key, scanlist);
//            if (tmp == scanlist.size()) {
//                scanlist.push_back(tmpp);
//            } else {
//                if (scanlist[tmp].time <= tmpp.time) {
//                    scanlist[tmp] = tmpp;
//                }
//            }
//        }
        uint64_t Key=values[i].key;
        std::string Value=vlog.getval(values[i].offset,values[i].vlen);
        if(values[i].vlen!=0) {
            Value = vlog.getval(values[i].offset, values[i].vlen);
        }
        else{
            Value = "";
        };
        uint32_t time=this->header.time;
        if(scanlist.count(Key)==0||scanlist[Key].size()==0) {
            scanlist[Key][time]=Value;
        }
        else {
            auto iter=scanlist[Key].end();
            iter--;
            if(iter->first<=time){
                if(iter->second!=delete_tag||iter->second!=""){
                    scanlist[Key].clear();
                    scanlist[Key][time]=Value;
                }
                else{
                    scanlist[Key].clear();
                }
            }
        }
        i++;
    }
}
long SSTable::findindex(uint64_t key, std::vector<scanstruct> scanlist) {
    int left = 0;
    int right = scanlist.size() - 1;
//    if(scanlist.empty()){
//        return 0;
//    }
//    if(scanlist.end()->_key<key){
//        return scanlist.size();
//    }
//    else {
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (scanlist[mid]._key == key) {
                return mid;// 返回指向该元素的指针
            }
            if (scanlist[mid]._key < key) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
//    }
   return left;
}

void SSTable::savefile()  {
    bool ifFileExists = false;
    uint64_t fileSize = 0;
    std::ifstream inFile(path, std::ios::in | std::ios::binary);

    if(inFile){
        ifFileExists = true;
        inFile.seekg(0, std::ios::beg);
        fileSize = inFile.tellg();
        inFile.close();
    }

    if(ifFileExists){
        return;
    }
    if(!ifFileExists){
        std::ofstream outFile(path, std::ios::out | std::ios::binary);
        outFile<<"";
        outFile.close();
    }

    std::fstream outFile(path, std::ios::out |  std::ios::binary);
    outFile.seekp(0,std::ios::beg);

    outFile.write((char*)&header.time, 8);
    outFile.write((char*)&header.count, 8);
    uint64_t length = header.count;
    outFile.write((char*)&header.min, 8);
    outFile.write((char*)&header.max, 8);
    //std::vector<unsigned char> buffer(FILTER_SIZE / 8, 0); // 默认初始化为0
    //for (size_t i = 0; i < buffer.size(); ++i) {
     //   for (size_t j = 0; j < 8 && i * 8 + j < FILTER_SIZE; ++j) {
     //       if (bloomFilter->bitSet[i * 8 + j]) {
     //           buffer[i] |= (1 << j);
     //       }
   //     }
  //  }
   //int rmp=buffer.size();
    outFile.seekp(8000,std::ios::cur);
    size_t tmp=outFile.tellp();

    for(int i=0;i<values.size();i++){
        outFile.write((char*)&values[i].key,8);
        outFile.write((char*)&values[i].offset,8);
        outFile.write((char*)&values[i].vlen,4);
    }
    outFile.close();
}

