#include "skiplist.h"
#include <optional>
#include <vector>
#define delete_tag "~DELETED~"
namespace skiplist {


skiplist_type::skiplist_type(double p) {
    coin=p;
    MAX_LEVEL=8;
    head=new node(MAX_LEVEL);
}
void skiplist_type::put(key_type key, const value_type &val) {
    node *p=head;
    node *update[MAX_LEVEL+1];
    for(int i=MAX_LEVEL-1;i>=0;i--){
        while(p->right[i]!=nullptr&&p->right[i]->key<key){
            p=p->right[i];
        }
        update[i]=p;
    }
    p=p->right[0];
    if(p!=nullptr&&p->key==key){
        p->data=val;
        return;
    }
    int level=0;
    while(rand()/(double)RAND_MAX<coin&&level<MAX_LEVEL-1){
        level++;
    }
    node *newnode=new node(level);
    for(int i=0;i<=level;i++){
        newnode->right[i]=update[i]->right[i];
        update[i]->right[i]=newnode;
    }
    newnode->data=val;
    newnode->key=key;
    //delete newnode;

}
void skiplist_type::print(){
    node *p=head;
    for(int i=MAX_LEVEL-1;i>=0;i--) {
        p=head;
        std::cout<<"level "<<i<<" ";
        while (p->right[i]!= NULL){
            std::cout<<p->lev<<" "<<p->key<<"---> ";
            p=p->right[i];
        }
        std::cout<<std::endl;
    }
}
std::string skiplist_type::get(key_type key) const {
    node *p=head;
    std::vector<node*> tmp;
    for(int i=MAX_LEVEL-1;i>=0;i--){
        while(p->right[i]!= nullptr&&p->right[i]->key<key){
            tmp.clear();
            for(int j=0;j<p->lev+1;j++){
                tmp.push_back(p->right[j]);
            }
            p=p->right[i];
        }
    }
    if(p->right[0]==NULL||p->right[0]->key!=key){
        return "";
    }
    else{
        return p->right[0]->data;
    }
}

bool skiplist_type::del(key_type key) {
    node *p=head;
    node *update[MAX_LEVEL+1];
    for(int i=MAX_LEVEL;i>=0;i--){
        while(p->right[i]!= nullptr&&p->right[i]->key<key){
            p=p->right[i];
        }
        update[i]=p;
    }
    p=p->right[0];
    if(p==nullptr||p->key!=key){
        return false;
    }
    else{
        for(int i=0;i<p->lev;i++){
            if(update[i]->right[i]!=p){
                break;
            }
            update[i]->right[i]=p->right[i];
        }
        delete p;
        return true;
    }
}

void skiplist_type::clear(){
    node *p=head->right[0];
    node *next;
    while (p != nullptr) {
        next = p->right[0];
        for (int i = 0; i < p->lev; ++i) {
            if (p->right[i] != nullptr) {
                p->right[i] = nullptr;
            }
        }
        delete p;
        p = next;
    }

    for (int i = 0; i < MAX_LEVEL; ++i) {
        head->right[i] = nullptr;
    }
}

std::list<std::pair<uint64_t, std::string>> skiplist_type::scan(int key1,int key2){
    std::list<std::pair<uint64_t, std::string>> sklist;
    node *p=head;
    for(int i=MAX_LEVEL-1;i>=0;i--){
        while(p->right[i]!= nullptr&&p->right[i]->key<key1){
            p=p->right[i];
        }
    }
    p=p->right[0];
    while(p->right[0]!= nullptr&&p->key<=key2){
        std::pair<uint64_t,std::string> tmp(p->key,p->data);
        if(p->data!=delete_tag){
            sklist.push_back(tmp);
        }
        p=p->right[0];
    }
    return sklist;
}

int skiplist_type::query_distance(key_type key) const {
    int count=0;
    node *p=head;
    for(int i=MAX_LEVEL;i>=0;i--){
        while(p->right[i]!= nullptr&&p->right[i]->key<key){
            p=p->right[i];
            count++;
        }
    }
    return count+1;
}


} // namespace skiplist
