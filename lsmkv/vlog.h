#include <datastructure.h>
#ifndef LSMKV_VLOG_H
#define LSMKV_VLOG_H
#include <vector>
#include "utils.h"
class Vlog {
public:
    std::vector<Entry> entrys;
    std::string fil_name;
//    void clear(){utils::rmfile(this->fil_name.c_str());}
    Vlog(const std::string dir):fil_name(dir){};
    Vlog(){};
    uint64_t current=0;
    uint64_t append(const uint64_t key,const std::string data);
    std::string getval(const uint64_t offset,const uint32_t vlen);
    int vlogval=0;
    std::vector<group> addtofile();
};

#endif //LSMKV_VLOG_H
