
#include <cstdint>
#include "tree.hpp"
#include <string>
class node{
public:
    node* son[4];
    int value;
    node(){
        for(int i=0;i<4;i++){
            son[i]= nullptr;
        }
    }
    int last_val;
};
class RadixTree : public Tree
{
private:
    node* root;
    int sizet=1;
    int heightt=0;
    // To Be Implemented
public:
    RadixTree();
    ~RadixTree();
    // basic operation
    void insert(int32_t value);
    void remove(int32_t value);
    void findsize(node* node,int &a);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};
