// implement a radix tree that support node compressino and store int32_t values
// each parent node has 4 children, representing 2 bits
#include <cstdint>
#include <algorithm>
#include "tree.hpp"
class Compressed_Node{
public:
    Compressed_Node* son[4];
    int value;
    int index_min=-1;
    int index_max=-1;
    Compressed_Node(){
        for(int i=0;i<4;i++){
            son[i]= nullptr;
        }
    }
    int last_val;
};
class CompressedRadixTree : public Tree
{
    // To Be Implemented
private:
    Compressed_Node* root;
    int sizet=1;
    int heightt=0;

public:
    CompressedRadixTree();
    ~CompressedRadixTree();
    // basic operation
    void insert(int32_t value);
    void Merge(Compressed_Node* node);
    void findheight(Compressed_Node* node,int b=0);
    void findsize(Compressed_Node* node,int &a);
    void remove(int32_t value);
    bool find(int32_t value);
    // statistics
    uint32_t size();
    uint32_t height();
};