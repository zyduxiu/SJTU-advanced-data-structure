#include "compressed_radix_tree.hpp"
#include <vector>
void BinaryRecursion(int n,std::vector<int>&ans) {

    for (int i = 31; i >= 0; i--) {
        ans.push_back(((n >> i) & 1));//与1做位操作，前面位数均为0
        //cout<<( (n>>i) & 1);//输出二进制

    }
}

void CompressedRadixTree::findsize(Compressed_node *node,int &a) {
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
void BinaryRecursion2(int n,std::vector<int>&ans)
{
    int a;
    a = n%2;
    n = n>>1;
    if (n != 0)
        BinaryRecursion(n,ans);
    //cout<<a;
    ans.push_back(a);
}
CompressedRadixTree::CompressedRadixTree()
{
    root=new Compressed_node();
    // To Be Implemented
}

void CompressedRadixTree::findheight(Compressed_node *node, int b) {
    int num=0;
    for(int i=0;i<4;i++){
        if(node->son[i]!= nullptr){
            num++;
        }
    }
    if(num==0){
        if(b>heightt){
            heightt=b;
        }
        return;
    }
    else{
        for(int i=0;i<4;i++){
            if(node->son[i]!= nullptr){
                b+=1;
                findheight(node->son[i],b);
                b-=1;
            }
        }
    }
}

CompressedRadixTree::~CompressedRadixTree()
{
    delete root;
    // To Be Implemented
}

void CompressedRadixTree::Merge(Compressed_node* node){
    int num=0;
    for(int i=0;i<4;i++){
        if(node->son[i]!=nullptr){
            num++;
        }
    }
    if(num==0){
        return;
    }
    if(num==1){
        for(int i=0;i<4;i++){
            if(node->son[i]!= nullptr){
                node->value=node->value<<(node->son[i]->index_max-node->son[i]->index_min)|node->son[i]->value;
                node->index_max=node->son[i]->index_max;
                node->last_val=node->son[i]->last_val;
                Compressed_node *st[4];
                for(int k=0;k<4;k++){
                    st[k]=node->son[i]->son[k];
                }
                node->son[i]= nullptr;
                for(int j=0;j<4;j++){
                    node->son[j] = st[j];
                    if(st[j]!= nullptr){
                        Merge(st[j]);
                    }
                    //Merge(node);
                }
                sizet--;
                return;
            }
        }
    }

}

void CompressedRadixTree::insert(int32_t value)
{
    Compressed_node* tmp=root;
    if(!find(value)) {

        unsigned int mask = 0xc0000000;
        for (int i = 0; i < 16; i++) {
            int val = (value & mask);
            val = val >> (30 - i * 2);
            mask = mask >> 2;
            if (tmp->son[val] == nullptr) {
                tmp->son[val] = new Compressed_node();
                int val2 = value << (i * 2);
                val2 = (unsigned) (val2) >> (i * 2);
                tmp->son[val]->value = val2;
                tmp = tmp->son[val];
                tmp->index_max = 32;
                tmp->index_min = i << 1;

                sizet++;
                break;
            } else {
                int val2 = value;

                std::vector<int> bitset1;
                std::vector<int> bitset2;
                BinaryRecursion(val2, bitset1);
                BinaryRecursion(tmp->son[val]->value, bitset2);
                tmp = tmp->son[val];
                int j = 0;
                bool whetherbreak = 0;
                for (j = 0; j < (tmp->index_max - tmp->index_min); j += 2) {
                    if (bitset1[tmp->index_min + j] != bitset2[32 + j - (tmp->index_max - tmp->index_min)] ||
                        bitset1[tmp->index_min + j + 1] != bitset2[33 + j - (tmp->index_max - tmp->index_min)]) {
                        whetherbreak = 1;
                        break;
                    }
                }
                if (whetherbreak == 1) {
                    int count = 0;
                    for (int cnt = tmp->index_min + j; cnt < 32; cnt += 1) {
                        count = count * 2 + bitset2[cnt];
                    }
                    int sig = tmp->last_val;
                    j = tmp->index_min + j;
                    tmp->value = tmp->value >> (tmp->index_max - j);

                    Compressed_node *newnode = new Compressed_node();
                    sizet++;
                    for (int p = 0; p < 4; p++) {
                        newnode->son[p] = tmp->son[p];
                    }
                    newnode->value = count;
                    newnode->last_val = sig;
                    newnode->index_min = j;
                    newnode->index_max = tmp->index_max;
                    for (int p = 0; p < 4; p++) {
                        tmp->son[p] = nullptr;
                    }
                    std::vector<int> bitset;
                    BinaryRecursion2(count, bitset);
                    val = count >> (tmp->index_max - j - 2) & 0x3;
                    //int ccnt=bitset2[0]*2+bitset2[1];
                    tmp->son[val] = newnode;
                    tmp->index_max = j;
                    tmp->last_val = 0;
                    sizet++;
                    mask = 0xc0000000 >> j;
                    i = j / 2 - 1;
                } else {
                    mask = 0xc0000000 >> (tmp->index_max);
                    i = (tmp->index_max) / 2 - 1;
                }
            }
        }
        tmp->last_val = value;
        int a = 0;
    }
    // To Be Implemented
}

void CompressedRadixTree::remove(int32_t value)
{
    if(find(value)) {
        Compressed_node *tmp = root;
        std::vector<bool> numpath;
        unsigned int mask = 0xc0000000;
        int val,i;
        for( i=0;i<16;i++){
            val=(value&mask);
            val=val>>(30-i*2);
            mask=mask>>2;
            int num=0;
            for(int k=0;k<4;k++){
                if(tmp->son[k]!= nullptr){
                    num++;
                }
            }
            if(num==1){
                numpath.push_back(0);
            }
            else{
                numpath.push_back(1);
            }
            tmp=tmp->son[val];
            i=(tmp->index_max/2)-1;
            mask=(0xc0000000)>>(tmp->index_max);
        }
        int k;
        for(k=numpath.size()-1;k>0;k--){
            if(numpath[k]==0&&numpath[k-1]==1){
                break;
            }
        }
        if(numpath[numpath.size()-1]==1){k=numpath.size()-1;}
        Compressed_node* newtmp=root;
        int cnnt=0;
        for(int i=0;i<k;i++){
            int val=(value&mask);
            val=val>>(30-(cnnt*2));
            if(mask!=3) {
                mask = mask >> 2;
            }
            newtmp=newtmp->son[val];
            cnnt=(newtmp->index_max/2);
            mask=(0xc0000000)>>(newtmp->index_max);
        }
        for(int i=k;i<numpath.size();i++){
            int val=(value&mask);
            val=val>>(30-(cnnt*2));
            mask>>=2;
            Compressed_node *ttmp=newtmp;
            newtmp=newtmp->son[val];
            cnnt=(newtmp->index_max/2);
            ttmp->son[val]= nullptr;
            sizet--;
            break;
        }
        newtmp=root;
        cnnt=0;
        for(int i=0;i<k;i++){
            int val=(value&mask);
            val=val>>(30-(cnnt*2));
            if(mask!=3) {
                mask = mask >> 2;
            }
            if(newtmp->son[val]== nullptr){
                break;
            }
            newtmp=newtmp->son[val];
            cnnt=(newtmp->index_max/2);
            mask=(0xc0000000)>>(newtmp->index_max);

        }
        Merge(newtmp);
    }
    return;
    // To Be Implemented
}

bool CompressedRadixTree::find(int32_t value)
{
    Compressed_node *tmp=root;
    unsigned int mask=0xc0000000;
    int i=0;
    int val;
    for( i=0;i<16;i++){
        val=(value&mask);
        val=val>>(30-i*2);
        mask=mask>>2;
        if(tmp->son[val]== nullptr){
            return false;
        }
        else{
            int val2=value;
            tmp=tmp->son[val];
            std::vector<int> bitset1;
            std::vector<int> bitset2;
            BinaryRecursion(val2,bitset1);
            BinaryRecursion(tmp->value,bitset2);
            for(int j=0;j<(tmp->index_max-tmp->index_min);j++) {
                if (bitset1[tmp->index_min + j] != bitset2[32 + j - (tmp->index_max - tmp->index_min)]) {
                    return false;
                }
            }
            i=(tmp->index_max/2)-1;
            mask=(0xc0000000)>>(tmp->index_max);
        }
    }
    // To Be Implemented
    return true;
}

uint32_t CompressedRadixTree::size()
{
    int sizeq=1;
    findsize(root,sizeq);
    return sizeq;
}

uint32_t CompressedRadixTree::height()
{
    heightt=1;
    findheight(root,1);
    // To Be Implemented
    return heightt;
}


