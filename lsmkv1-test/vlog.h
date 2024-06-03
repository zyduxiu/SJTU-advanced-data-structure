#include "datastructure.h"
#ifndef LSMKV_VLOG_H
#define LSMKV_VLOG_H
#include <vector>
#include "utils.h"
class Vlog {
public:
    std::vector<Entry*> entrys;
    std::string fil_name;
    uint64_t tail=0;
    void clear(){utils::rmfile(this->fil_name.c_str());  entrys.clear();}
    Vlog(const std::string dir):fil_name(dir){};
    Vlog(){entrys.reserve(500);};
    uint64_t current=0;
    uint64_t append(std::vector<Entry*> t);
    std::string getval(const uint64_t offset,const uint32_t vlen);
    uint64_t getkey(const uint64_t offset);
    uint32_t getvlen(const uint64_t offset);
    int vlogval=0;
    std::vector<group> addtofile();
};

#endif //LSMKV_VLOG_H
