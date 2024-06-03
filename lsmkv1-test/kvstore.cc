#include "kvstore.h"
#include <string>
#include  <fstream>
#include "skiplist.h"
#include "vlog.h"
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <map>

#include "utils.h"
#define mem_already_deleted "~![ERROR] ALREADY DELETED!~"
#define mem_not_exist "~![ERROR] MemTable No Exist!~"
#define delete_tag "~DELETED~"
#define sstvalue_noFound "~![ERROR] data not found!~"


int max_sst(int lev){
    return (0x1<<(lev+1));
}


KVStore::KVStore(const std::string &dir, const std::string &vlog) : KVStoreAPI(dir,vlog)
{

    
    t_dir=dir;
    t_vlog=vlog;
    Vlog newvlog(t_vlog);
    vlog1=newvlog;
    timestamp=0;
    seektail(&vlog1);
    
    this->sstcheck(t_dir);
    this->mmtable=new memtable;

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
   if(this->mmtable->Spacesize!=0){
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::microseconds mstime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    this->timestamp++;
    std::string newFilePath = this->t_dir + "/level-0/" + std::to_string(mstime.count()) + ".sst";
    SSTable* curr=loadtofile(timestamp,newFilePath);
    this->saveCache[0][mstime.count()] = curr;
    this->mmtable->reset();
    //for compaction
    int result=this->whichmerge();
    while(result!=-1){//一直merge直到所有sstlev不需要merge
        this->Compaction(result);
        result=this->whichmerge();
    }//把跳表直接放入
    }
   delete this->mmtable;
    for(auto level = this->saveCache.begin(); level != this->saveCache.end(); level++){
       for(auto sstable = level->second.begin(); sstable != level->second.end(); sstable++){
            delete sstable->second;
        }
   }
    // Delete the vlog
 //   this->vlog1.clear();
    this->saveCache.clear();
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */

SSTable* KVStore::loadtofile(uint64_t timestamp,std::string path) {
    skiplist::node *p=mmtable->list->head;
    p=p->right[0];
    std::vector<Entry*> tmpp;
    tmpp.reserve(500);
    while(p!= nullptr){
        Entry* tmppp=new Entry(p->key,p->data.size(),p->data);
        tmpp.push_back(tmppp);
        p=p->right[0];
    }
    vlog1.append(tmpp);
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
    int result=this->whichmerge();
    while(result!=-1){//一直merge直到所有sstlev不需要merge
        this->Compaction(result);
        result=this->whichmerge();
    }

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
    int delete_times=0;
    int put_times=0;
    if(val!=mem_already_deleted&&val!=mem_not_exist){
        return val;
    }
    if(val==mem_already_deleted||val==delete_tag){
        return "";
    }
    if(val != mem_not_exist)
        return val;
    else{
        uint64_t Timestamp=0;
        bool ifFound=false;
        std::string validx;
        for(int level=this->saveCache.size()-1; level>=0; level--){
            for(auto sstable = saveCache[level].begin(); sstable!=saveCache[level].end();sstable++) {
                SSTable *currentSSTable = sstable->second;
                if((key<currentSSTable->header.min||key>currentSSTable->header.max)
                ||!currentSSTable->bloomFilter->contains(key)
                   ){
                    continue;//bloomfilter;
                }
                if(currentSSTable->header.time>=Timestamp&&currentSSTable->getfromcache(key,&vlog1)!= sstvalue_noFound ){
                     validx=currentSSTable->getfromcache(key,&vlog1);
                        Timestamp=currentSSTable->header.time;
                        ifFound=true;
                }
            }
        }
        if(ifFound&&validx!=delete_tag||validx!=""){
            return validx;
        }
        if(!ifFound){
            return "";
        }
        if(ifFound&&validx==delete_tag||validx==""){return  "";}
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
    for(int level=1; level < this->saveCache.size(); level++){
        for(auto sstable = saveCache[level].begin(); sstable!=saveCache[level].end();sstable++){
            sstable->second->clear();
            delete sstable->second;
        }
        std::string levelpath=this->t_dir+"/level-"+std::to_string(level);
        utils::rmdir(levelpath.c_str());
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
    list1.clear();
    std::map<uint64_t, std::map<uint32_t,std::string>> tmp;
    std::list<std::pair<uint64_t, std::string>> skipmap;
    mmtable->scan(key1,key2,skipmap);
    for(int level=0; level < this->saveCache.size(); level++){
        for(auto sstable = saveCache[level].begin(); sstable!=saveCache[level].end();sstable++){
            SSTable* cursst=sstable->second;
            cursst->scan(key1,key2,tmp,vlog1);
        }
    }

//    for ( auto iter=tmp.begin();iter!=tmp.end();iter++) {
//        bool found = false;
//        for (auto it = list1.begin(); it != list1.end(); ++it) {
//            if (it->first == iter->first) {
//                found = true;
//                break;
//            }
//        }
//        auto itery=iter->second.end();
//        itery--;
//        if (!found&&itery->second!=delete_tag) {
//            list1.emplace_back(iter->first, itery->second);
//        }
//    }
//    std::vector<std::pair<uint64_t, std::string>> vec(list1.begin(), list1.end());
//    std::sort(vec.begin(), vec.end(),
//              [](const std::pair<uint64_t, std::string>& a, const std::pair<uint64_t, std::string>& b) {
//                  return a.first < b.first;
//              }
//    );
    for(auto x=tmp.begin();x!=tmp.end();x++){
        uint64_t curkey=x->first;
        for(auto y=x->second.begin();y!=x->second.end();y++){
            list1.push_back({curkey,y->second});
        }
    }

    for (auto it = skipmap.begin(); it != skipmap.end(); ) {
        uint64_t findkey = it->first;
        auto itx = std::find_if(list1.begin(), list1.end(), [&findkey](const std::pair<uint64_t, std::string>& pair) {
            return pair.first == findkey;
        });
        if (itx != list1.end()) {
            if (it->second != delete_tag&&it->second!=mem_already_deleted&&it->second!=mem_not_exist) {
                std::pair<uint64_t, std::string> tmp = {it->first, it->second};
                list1.erase(itx); // 更新 itx 为 erase 返回的迭代器
                list1.push_back(tmp);
            } else {
                list1.erase(itx); // 更新 itx 为 erase 返回的迭代器
            }
        } else {
            list1.push_back({findkey, it->second});
            ++it; // 只有在没有删除元素时才前进迭代器
        }
    }

    for (auto it = list1.begin(); it != list1.end(); ) {
        if (it->second == delete_tag || it->second.size() == 0) {
            it = list1.erase(it);
        } else {
            ++it;
        }
    }


}


void KVStore::seektail(Vlog* vlog){
    std::vector<std::string> this2;
    if(utils::scanDir(t_dir,this2)==0){
    std::cout<<"here";
    	return ;
    	}
    uint64_t curhole=utils::seek_data_block(this->t_vlog);
    if(curhole==0){
    	vlog->tail=0;
    	return;
    }
    std::ifstream inFile(t_vlog,std::ios::in|std::ios::binary);
    inFile.seekg(curhole,std::ios::beg);
    uint8_t magic=0;
    uint16_t check=0;
    uint16_t seccheck=-1;
    while(!(magic==0xff&&check==seccheck)){
         inFile.read(reinterpret_cast<char*>(&magic),sizeof(magic));
         if(magic!=0xff){
            inFile.seekg(1,std::ios::cur);
            curhole++;
            //std::cout<<magic<<std::endl;
            continue;
         }
         else{
             size_t m=inFile.tellg()-1;
             uint64_t key;
             uint32_t vlen;
             std::string value="";
             inFile.read(reinterpret_cast<char*>(&check),sizeof(check));
             inFile.read(reinterpret_cast<char*>(&key),sizeof(key));
             inFile.read(reinterpret_cast<char*>(&vlen),sizeof(vlen));
             char* buffer=new char[vlen];
             inFile.read(buffer,vlen);
             for(size_t i=0;i<vlen;i++){
                value+=buffer[i];
             }
           //  std::cout<<check<<" "<<key<<" "<<value<<std::endl;
             std::vector<unsigned char> data;
             unsigned char* keyPtr=reinterpret_cast<unsigned char*>(&key);
    	    for(size_t i=0;i<sizeof(key);i++){
    	        data.push_back(keyPtr[i]);
    	    }
    	    unsigned char* vlenPtr=reinterpret_cast<unsigned char*>(&vlen);
    	    for(size_t i=0;i<sizeof(vlen);i++){
    	        data.push_back(vlenPtr[i]);
    	    }
    	    for(const char& c:value){
    	        data.push_back(static_cast<unsigned char>(c));
    	    }
            seccheck=utils::crc16(data);
            if(seccheck==check){
            	//int cur=curhole-1;
                (*vlog).tail=m;
               // std::cout<<"tail "<<vlog->getvlen(curhole);
                break;
            }
            else{
                curhole++;
                inFile.seekg(curhole,std::ios::beg);
            }
         }

    }
return;
}


void KVStore::sstcheck(std::string path){
std::string dataDir;
if(path[path.length()] == '/')
    dataDir = path.substr(0, path.length() - 1);
else
    dataDir = path;
if(!utils::dirExists(path)){
    utils::mkdir((path+"/level-0").c_str());  
    return;
}
std::vector<std::string> levelNames;
int levelNum = utils::scanDir(path, levelNames);
if (levelNum > 0) {
    for(int i = 0; i < levelNum; ++i) {
        std::string levelName = "level-" + std::to_string(i);
        // check if the level directory exists
        if(std::count(levelNames.begin(), levelNames.end(), levelName) == 1) {
            std::string levelDir = dataDir + "/" + levelName;
            std::vector<std::string> tableNames;
            int tableNum = utils::scanDir(levelDir, tableNames);
            
            for(int j = 0; j < tableNum; ++j) {
                SSTable* curCache = new SSTable(levelDir + "/" + tableNames[j]);
                uint64_t curTime = (curCache->header).time;
                int len=tableNames[j].size();
                std::string sstname=tableNames[j].substr(0,len-4);
                uint64_t num1=std::stoull(sstname);
                saveCache[i][num1]=curCache;
                if(curTime > this->timestamp)
                    timestamp = curTime;
            }
            // make sure the timeStamp of cache is decending
        } else
            break;
    }
    
} else {
    utils::mkdir((dataDir + "/level-0").c_str());
}


}
uint64_t KVStore::whichmerge(){
    for(int level=0;level<saveCache.size();level++){
        if(saveCache[level].size()> max_sst(level)){
            return level;
        }
    }
    return -1;
}

void KVStore::Compaction(uint64_t level) {
    if(level>=this->saveCache.size()-1){//创建一个更高的级别
        std::string levelpath=this->t_dir+"/level-"+std::to_string(level+1);
        utils::mkdir(levelpath.c_str());
    }
    //step1: 选择进行合并的SSTables;
    std::map<uint64_t,std::map<uint64_t ,SSTable *>> sstSelect;
    if(level==0){//如果要选择0级SST；
        for(auto ssttable=this->saveCache[0].begin();ssttable!=saveCache[0].end();ssttable++){
            sstSelect[level][ssttable->first]=ssttable->second;
        }
    }else{//level大于0的情况？
        uint64_t selectnum=saveCache[level].size()- max_sst(level);//需要选的sst数
        uint64_t choosednum=0;//已经选中要合并的sst数量
        std::map<uint64_t,std::map<uint64_t,SSTable*>>sortSST;
        std::map<uint64_t,std::map<uint64_t ,uint64_t >>NameMap;//根据时间戳排序
        for(auto sst=this->saveCache[level].begin();sst!=this->saveCache[level].end();sst++){
            SSTable* cursst=sst->second;
            uint64_t curTime=cursst->header.time;
            uint64_t minkey=cursst->header.min;
            sortSST[curTime][minkey]=cursst;
            NameMap[curTime][minkey]=sst->first;
        }
        for(auto sstlev=sortSST.begin();sstlev!=sortSST.end();sstlev++){
            for(auto sst=sstlev->second.begin();sst!=sstlev->second.end();sst++){
                if(choosednum<selectnum){
                    sstSelect[level][NameMap[sstlev->first][sst->first]]=sst->second;
                    choosednum++;
                }
            }
        }
    }
    //step2:选择被合并级的SST
    if(sstSelect[level].size()>0){
        uint64_t levelmin=UINT64_MAX;
        uint64_t levelmax=0;
        for(auto sst=sstSelect[level].begin();sst!=sstSelect[level].end();sst++){
            if(sst->second->header.min<levelmin){
                levelmin=sst->second->header.min;
            }
            if(sst->second->header.max>levelmax){
                levelmax=sst->second->header.max;
            }//得到选择的SSTavle里的minkey和maxkey,
        }
        for(auto sst=saveCache[level+1].begin();sst!=saveCache[level+1].end();sst++){//遍历被合并的级的savecache
            SSTable *cursst=sst->second;
            uint64_t curmin=cursst->header.min;
            uint64_t curmax=cursst->header.max;
            if(curmax>=levelmin&&curmin<=levelmax){
                sstSelect[level+1][sst->first]=cursst;//如果键正好被包含，被select
            }
        }
    }
    //step3 合并选中的sstables
    std::vector<SSTable*>merge;
    std::map<uint64_t ,std::map<uint64_t ,std::map<uint64_t,uint32_t>>> offsetMap;//offsetMap[key][timestamp]={vlen,offset}
    uint64_t updatetime=0;
    for(auto sstlev=sstSelect.rbegin();sstlev!=sstSelect.rend();sstlev++){
        for(auto sst=sstSelect[sstlev->first].begin();sst!=sstSelect[sstlev->first].end();sst++){
            SSTable* cursst=sst->second;
            merge.push_back(cursst);
            if(cursst->header.time>updatetime){
                updatetime=cursst->header.time;
            }
        }
    }//将SSTselect放入merge;
    for(uint64_t i=0;i<merge.size();i++){
        SSTable *cursst=merge[i];
        uint64_t num=cursst->header.count;
        for(uint64_t j=0;j<num;j++){
            uint64_t curkey=cursst->values[j].key;
            uint64_t curoffset=cursst->values[j].offset;
            uint32_t curvlen=cursst->values[j].vlen;
            uint64_t curtime=cursst->header.time;
            if(offsetMap.find(curkey)!=offsetMap.end()){
                auto it=offsetMap[curkey].end();
                it--;
                if(it->first<=curtime){
                    auto ity=it->second.end();
                    ity--;
//                    if(ity->second==0&&level==saveCache.size()-1){
//                        offsetMap[curkey].clear();
//                    }
//                    else {
                        offsetMap[curkey].clear();
                        offsetMap[curkey][curtime][curoffset] = curvlen;
//                    }
                }
            }
            else {
                offsetMap[curkey][curtime][curoffset] = curvlen;
            }
        }
    }

    //删除已删除的键 offsetMap(key timestamp offset vlen
//    std::map<uint64_t,std::map<uint64_t,std::map<uint64_t ,uint32_t >>>deletedmap;
//    for(auto key=offsetMap.begin();key!=offsetMap.end();key++){
//        auto value1=key->second.end();
//        value1--;//map(time:map(offset,vlen ))
//        auto pair=value1->second.end();
//        pair--;
//        //pair(offset,vlen)
//        if(pair->second==0){//这个键值已经删除了
//            continue;//跳过即删除
//        }
//        deletedmap[key->first][value1->first][pair->first]=pair->second;
//    }
    std::vector<group> newgroups;
    uint64_t curamount=0;
    for(auto it=offsetMap.begin();it!=offsetMap.end();it++){
        group newindex;
        newindex.key=it->first;
        newindex.offset=it->second.begin()->second.begin()->first;
        newindex.vlen=it->second.begin()->second.begin()->second;
        curamount++;
        if(curamount<=406){
            newgroups.push_back(newindex);
        }else{//大于的情况直接保存目前的sstable
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            std::chrono::microseconds nstime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
            std::string newFilePath = this->t_dir + "/level-" + std::to_string(level+1) + "/" + std::to_string(nstime.count()) + ".sst";

            SSTable *newSSTable =new SSTable(t_vlog,newFilePath,newgroups,updatetime);
            int current = this->saveCache[level+1].size();
            this->saveCache[level+1][nstime.count()] = newSSTable;
            // 重置groups和目前计数
            newgroups.clear();
            curamount=0;
            newgroups.push_back(newindex);
            curamount++;
        }
    }
    if(newgroups.size()>0){//保存剩余的sstable
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::microseconds nstime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
        std::string newFilePath = this->t_dir + "/level-" + std::to_string(level+1) + "/" + std::to_string(nstime.count()) + ".sst";
        SSTable* newSSTable =new SSTable(t_vlog,newFilePath,newgroups,updatetime);
        
        int current = this->saveCache[level+1].size();
        this->saveCache[level+1][nstime.count()] = newSSTable;

        // Reset the entriyMap and listSSTfileSize
        newgroups.clear();
       // curamount=0;
    }
    for (int curlev = 0; curlev < sstSelect.size(); curlev++) {
        for (auto cur = sstSelect[curlev].begin(); cur != sstSelect[curlev].end(); /* no increment here */) {
            SSTable *cursst = cur->second;
            if (cursst != NULL) {
                for (int outerIter = 0; outerIter < saveCache.size(); outerIter++) {
                    auto innerIter = saveCache[outerIter].begin();
                    while (innerIter != saveCache[outerIter].end()) {
                        if (innerIter->second == cursst) {
                            innerIter = saveCache[outerIter].erase(innerIter);
                        } else {
                            ++innerIter;
                        }
                    }
                }
                cursst->clear();
                // Now that we've finished processing cursst, we can move to the next element.
                ++cur;
            }
            // If the current level has only one SSTable left, you may want to erase it.
            //else if (saveCache[curlev].count(cur->first) == 1) {
            //    saveCache[curlev].erase(cur->first);
            //}
        }
    }
//    std::map<uint64_t ,std::map<uint64_t ,
//    if(level==saveCache.size()-1&&saveCache[level+1].size()!=0){
//        for(auto it=saveCache[level+1].begin();it!=saveCache[level+1].end();it++){
//
//        }
//    }

//
//for(auto sst1=saveCache[level+1].begin();std::next(sst1)!=saveCache[level+1].end();sst1++){
//    auto sst=std::next(sst1);
//    for(auto sst2=sst;sst2!=saveCache[level+1].end();sst2++){
//        if((sst2->second->header.max>=sst1->second->header.min)&&(sst2->second->header.min<=sst1->second->header.max)&&
//                    (sst2->second->header.time==sst1->second->header.time)){
//            for(int k=0;k< sst2->second->values.size();k++){
//                uint64_t curkey= sst2->second->values[k].key;
//                uint64_t curoffset= sst2->second->values[k].offset;
//                uint32_t curvlen= sst2->second->values[k].vlen;
//                if(! (sst1->second->bloomFilter->contains(curkey))){
//                    continue;
//                }
//                else{
//                    for(int h=0;h<sst1->second->values.size();h++){
//                        if(sst1->second->values[h].key==curkey){
//                            sst1->second->values[h].vlen=curvlen;
//                            sst1->second->values[h].offset=curoffset;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
	return;
}
/**
 * This reclaims space from vLog by moving valid value and discarding invalid value.
 * chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size)
{
   
    std::map<uint64_t, std::map<uint64_t ,std::map<std::string,uint32_t>>>offsetmap;//key,offset,value
    uint64_t curtail=this->vlog1.tail+15;
    uint64_t cursize=0;
    uint64_t curoffset=curtail;
    int numcount=0;
   // uint64_t end=utils::seek_data_block(this->t_vlog)+chunk_size;
    while(cursize<chunk_size){
        uint64_t curkey=vlog1.getkey(curoffset);
        uint32_t curvlen=vlog1.getvlen(curoffset);
        std::string curvalue=vlog1.getval(curoffset,curvlen);
        offsetmap[curkey][curoffset][curvalue]=curvlen;
    //    std::cout<<curkey<<" "<<curoffset<<" "<<curvalue<<" "<<curvlen<<std::endl;
        curoffset+=curvlen+15;
        cursize+=curvlen+15;
        numcount++;
    }
	uint64_t curkey=vlog1.getkey(curoffset);
        uint32_t curvlen=vlog1.getvlen(curoffset);
        std::string curvalue=vlog1.getval(curoffset,curvlen);
        offsetmap[curkey][curoffset][curvalue]=curvlen;
        curoffset+=curvlen+15;
        cursize+=curvlen;
    for(auto it=offsetmap.begin();it!=offsetmap.end();it++){
        uint64_t key=it->first;
        std::string latestvalue=this->get(key);//scan the vlog
        auto itx=it->second.begin()->second.begin();
        if(latestvalue==itx->first){// if key is valid
            this->put(key,latestvalue);
        }
    }
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::microseconds mstime = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    this->timestamp++;
    std::string newFilePath = this->t_dir + "/level-0/" + std::to_string(mstime.count()) + ".sst";
    SSTable* curr=loadtofile(timestamp,newFilePath);
    this->saveCache[0][mstime.count()] = curr;
    this->mmtable->reset();
    //for compaction
     int result=this->whichmerge();
     while(result!=-1){//一直merge直到所有sstlev不需要merge
         this->Compaction(result);
         result=this->whichmerge();
     }//把跳表直接放入
//    dig holes
    //std::cout<<vlog1.tail<<std::endl;
    uint64_t newtail=utils::seek_data_block(this->t_vlog);
    vlog1.tail=curoffset-15;
    utils::de_alloc_file(this->t_vlog,newtail,chunk_size);
   
   
    //lastblock=newtail;
   // std::cout<<newtail<<std::endl;
    //for(auto itx=offsetmap.rbegin();itx!=offsetmap.rend();itx++){
      //  for(auto ity=itx->second.rbegin();ity!=itx->second.rend();ity++){
        //    auto val=ity->second.begin()->first;
           // auto vlenv=&ity->second.begin()->second.begin()->first;
          //auto vlen=ity->second.begin()->second.begin()->first;
           // auto offval=ity->first;
           // if(offval-15>=newtail) {
             //   newtail = ity->first - 15;
               // std::vector<unsigned char> data;
              //  const unsigned char *keyPtr = reinterpret_cast<const unsigned char *>(&itx->first);
              //  const unsigned char *vlenPtr = reinterpret_cast<const unsigned char *>(vlenv);
              //  data.insert(data.end(), keyPtr, keyPtr + sizeof(itx->first));
              //  data.insert(data.end(), vlenPtr, vlenPtr + 4);
              //  data.insert(data.end(), val.begin(), val.end());
              //  uint16_t checksum = utils::crc16(data);
              //  uint8_t curmagic = offsetmap[itx->first][ity->first][val][vlen].begin()->first;
               // uint16_t curcheck=offsetmap[itx->first][ity->first][val][vlen].begin()->second;
               // utils::de_alloc_file(this->t_vlog,this->vlog1.tail,newtail);
              // if(curmagic==0xff&&curcheck==checksum){
               // vlog1.tail=newtail;
               // std::cout<<vlog1.tail<<std::endl;
               // break;     
               // }
           // }
      //  }
   // }
    
    
}

