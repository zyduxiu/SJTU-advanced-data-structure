//
// Created by zhangyue on 2024/4/15.
//
#include "vlog.h"
#include <cstring>
#include <cstdint>
#include <fstream>
//#include "utils.h"
// 假设key和vlen都是int类型，value是std::string类型
std::vector<unsigned char> KeyValueToBinary(int key, int vlen, const std::string& value) {
    std::vector<unsigned char> binary_data;
    binary_data.reserve(sizeof(key) + sizeof(vlen) + value.size());
    binary_data.insert(binary_data.end(), reinterpret_cast<char*>(&key), reinterpret_cast<char*>(&key) + sizeof(key));
    binary_data.insert(binary_data.end(), reinterpret_cast<char*>(&vlen), reinterpret_cast<char*>(&vlen) + sizeof(vlen));
    binary_data.insert(binary_data.end(), value.begin(), value.end());
    return binary_data;
}



uint64_t Vlog::append(const uint64_t key, const std::string data) {

//    short tmp= utils::crc16(KeyValueToBinary(key,data.size(),data));
    short tmp=1;
    Entry entry(tmp,key,data.size(),data);
    entrys.push_back(entry);
    vlogval++;
    return 0;
//    current+=(15+data.size());
//    return ctmp;
}

std::vector<group> Vlog::addtofile() {
    bool ifFileExists = false;
    uint64_t fileSize = 0;
    std::ifstream inFile(fil_name, std::ios::in | std::ios::binary);

    if(inFile){
        ifFileExists = true;
        inFile.seekg(0, std::ios::end);
        fileSize = inFile.tellg();
        inFile.close();
    }

    if(!ifFileExists){
        std::ofstream outFile(fil_name, std::ios::out | std::ios::binary);
        outFile<<"";
        outFile.close();
    }
//    std::ifstream iFile(fil_name, std::ios::in | std::ios::binary);
    std::vector<group> groups;
    std::fstream outFile(fil_name, std::ios::out | std::ios::in | std::ios::binary);
    outFile.seekg(0,std::ios::end);

    int current=0;
    for(int i=0;i<entrys.size();i++){
        Entry entry = this->entrys[i];
        size_t len = entry.val.length();
        char char_array[len + 1];
        std::copy(entry.val.begin(), entry.val.end(), char_array);
        char_array[len] = '\0';
        current=outFile.tellg();
        current+=15;
        outFile.write((char*)&entry.Magic, sizeof(uint8_t));
        outFile.write((char*)&entry.Checksum, sizeof(uint16_t));
        outFile.write((char*)&entry.key, sizeof(uint64_t));
        outFile.write((char*)&entry.vlen, sizeof(uint32_t));
        outFile.write((char*)&char_array, entry.val.size());


        group tgroup;
        tgroup.offset=current;
        tgroup.key=entrys[i].key;
        tgroup.vlen=entrys[i].vlen;
        groups.push_back(tgroup);
    }
    outFile.close();
    entrys.clear();
    return groups;
}

std::string Vlog::getval( uint64_t offset, uint32_t length){
    std::ifstream inFile(fil_name, std::ios::in | std::ios::binary);

    inFile.seekg(0, std::ios::end);
    size_t fileSize = inFile.tellg();

    if(offset > fileSize || (offset + length) > fileSize){
        inFile.close();
        return "";
    }

    inFile.seekg(offset, std::ios::beg);

    size_t ss=inFile.tellg();
    char* buffer = new char[length];
    inFile.read(buffer, length);

    std::string res = "";
    for(size_t i = 0; i < length; i++)
        res += buffer[i];
    inFile.close();
    delete[] buffer;
    return res;
}