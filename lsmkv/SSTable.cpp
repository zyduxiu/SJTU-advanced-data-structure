
// Created by zhangyue on 2024/4/8.
//
#include "SSTable.h"
#include  <fstream>
#define sstvalue_noFound "~![ERROR] data not found!~"

void SSTable::loadfile(std::string dir) {
    std::ifstream file(dir, std::ios::binary);
    if(!file) {
        printf("Fail to open file %s", dir.c_str());
        exit(-1);
    }
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0,std::ios::beg);
    file.read((char*)&header.time, 8);
    file.read((char*)&header.count, 8);
    uint64_t length = header.count;
    file.read((char*)&header.min, 8);
    file.read((char*)&header.max, 8);
    char *filterBuf = new char[2000];
    file.read(filterBuf, 2000);
    bloomFilter = new BloomFilter(filterBuf);
    char *indexBuf = new char[length * 20];
    file.read(indexBuf, length * 20);
    for(uint32_t i = 0; i < length; ++i) {
        values.push_back(group(*(uint64_t*)(indexBuf + 20*i), *(uint64_t*)(indexBuf + 20*i + 8),*(uint32_t*)(indexBuf+20*i+16)));
    }

//    delete[] filterBuf;
    delete[] indexBuf;
    file.close();
}



std::string SSTable::getfromcache(uint64_t key,Vlog vlog){
    //先跳过bloomfilter部分
    for(int i=0;i<values.size();i++){
        if(key==values[i].key){
            return vlog.getval(values[i].offset,values[i].vlen);
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
    std::vector<unsigned char> buffer(FILTER_SIZE / 8, 0); // 默认初始化为0
    for (size_t i = 0; i < buffer.size(); ++i) {
        for (size_t j = 0; j < 8 && i * 8 + j < FILTER_SIZE; ++j) {
            if (bloomFilter->bitSet[i * 8 + j]) {
                buffer[i] |= (1 << j);
            }
        }
    }
//    int rmp=buffer.size();
    outFile.write((char *)&buffer, 2000);
    size_t tmp=outFile.tellp();

    for(int i=0;i<values.size();i++){
        outFile.write((char*)&values[i].key,8);
        outFile.write((char*)&values[i].offset,8);
        outFile.write((char*)&values[i].vlen,4);
    }
    outFile.close();
}

