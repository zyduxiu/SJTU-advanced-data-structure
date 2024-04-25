#include "radix_tree.hpp"

RadixTree::RadixTree()
{
    root=new node();
    // To Be Implemented
}



void RadixTree::findsize(node *node,int &a) {
    int num=0;
    for(int i=0;i<4;i++){
        if(node->son[i]!= nullptr){
            num++;
        }
    }
    if(num==0){return;}
    int num1=0;
    for(int i=0;i<4;i++){
        if(node->son[i]!= nullptr){
            num1++;
        }
    }
    a+=num1;
    for(int i=0;i<4;i++){
        if(node->son[i]!= nullptr){
            findsize(node->son[i],a);
        }
    }

}

RadixTree::~RadixTree()
{
    delete root;
    // To Be Implemented
}

void RadixTree::insert(int32_t value)
{
    node* tmp=root;
    unsigned int mask=0xc0000000;
    for(int i=0;i<16;i++){
        int val=(value&mask);
        val=val>>(30-i*2);
        mask=mask>>2;
        if(tmp->son[val]== nullptr){
            tmp->son[val]=new node();
            tmp=tmp->son[val];
            tmp->value=val;
            sizet++;
        }
        else{
            tmp=tmp->son[val];
        }
    }
    tmp->last_val=value;
    heightt=17;
    // To Be Implemented
}

void RadixTree::remove(int32_t value)
{
    if(!find(value)) {
        bool sizes[16];
        node *tmp = root;
        if (find(value)) {
            unsigned int mask = 0xc0000000;
            for (int i = 0; i < 16; i++) {
                int val = (value & mask);
                val = val >> (30 - (i * 2));
                mask = mask >> 2;
                int size = 0;
                for (int cnt = 0; cnt < 4; cnt++) {
                    if (tmp->son[cnt] != nullptr) {
                        size++;
                    }
                }
                if (size == 1) {
                    sizes[i] = 0;
                } else {
                    sizes[i] = 1;
                }
                tmp = tmp->son[val];
            }
            int k;
            for (k = 15; k > 0; k--) {
                if (sizes[k] == 0 && sizes[k - 1] == 1) {
                    break;
                }
            }
            k--;
            if (sizes[15] == 1) { k = 15; }
            if (k == 0) {
                heightt = 0;
            }
            node *newtmp = root;
            mask = 0xc0000000;
            for (int i = 0; i < k; i++) {
                int val = (value & mask);
                val = val >> (30 - (i * 2));
                if (mask != 3) {
                    mask = mask >> 2;
                }
                newtmp = newtmp->son[val];
            }
            for (int i = k; i < 16; i++) {
                int val = (value & mask);
                val = val >> (30 - (i * 2));
                node *ttmp = newtmp;
                newtmp = newtmp->son[val];
                sizet--;
                ttmp->son[val] = nullptr;
                break;
            }

        }
    }
    // To Be Implemented
}

bool RadixTree::find(int32_t value)
{
    node* tmp=root;
    unsigned int mask=0xc0000000;
    for(int i=0;i<16;i++){
        int val=(value&mask);
        val=val>>(30-(i*2));
        mask=mask>>2;
        if(tmp->son[val]== nullptr){
            return false;
        }
        else{
            tmp=tmp->son[val];
        }

    }
    // To Be Implemented
    return true;
}

uint32_t RadixTree::size()
{
    int sizeq=1;
    findsize(root,sizeq);
    return sizeq;
    // To Be Implemented
    return 0;
}

uint32_t RadixTree::height()
{
    return 17;
    // To Be Implemented
    return 0;
}
