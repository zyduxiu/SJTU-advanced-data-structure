#include "hftree.h"
#include <vector>
hfTree::hfTree(const std::string &text, const Option op)
{
    std::vector<node> map;
    std::vector<node> map2;
    bool existed=false;
    if(op==Option::SingleChar){
        for(int i=0;i<text.size();i++){
            if(map.size()==0){
                node tmp;
                tmp.data=text[i];
                tmp.weight=1;
                map.push_back(tmp);
            }//初始size为空的情况下
            else{
                for(int j=0;j<map.size();j++){
                    if(map[j].data[0]==text[i]){
                        map[j].weight++;
                        existed=true;
                    }
                }
                if(existed==true){
                    existed=false;
                }
                else{
                    node tmp;
                    tmp.data=text[i];
                    tmp.weight=1;
                    map.push_back(tmp);
                }
            }
        }
        while(map.size()!=1){
            int weight1=99999;
            int weight2=99999;
            std::string data1,data2;
            int pos1,pos2;
            for(int i=0;i<map.size();i++){
                if(map[i].weight<=weight1){
                    if(map[i].weight==weight1&&data1>findleast(&map[i])){
                        pos1=i;
                        data1=findleast(&map[i]);
                    }
                    if(map[i].weight<weight1){
                        pos1=i;
                        data1=findleast(&map[i]);
                        weight1=map[i].weight;
                    }
                }
            }
            for(int i=0;i<map.size();i++){
                if(map[i].weight<=weight2){
                    if(map[i].weight==weight2&&data2>findleast(&map[i])&&i!=pos1){
                        pos2=i;
                        data2=findleast(&map[i]);
                    }
                    if(map[i].weight<weight2&&i!=pos1){
                        pos2=i;
                        data2=findleast(&map[i]);
                        weight2=map[i].weight;
                    }
                }
            }
            node* left=new node;
            left->weight=map[pos1].weight;
            left->data=map[pos1].data;
            left->left=map[pos1].left;
            left->right=map[pos1].right;
            node* right=new node;
            right->weight=map[pos2].weight;
            right->data=map[pos2].data;
            right->left=map[pos2].left;
            right->right=map[pos2].right;
            map.erase(map.begin()+pos1);
            if(pos1<pos2){
                pos2--;
            }
            map.erase(map.begin()+pos2);
            node newnode;
            newnode.left=left;
            newnode.right=right;
            newnode.weight=left->weight+right->weight;
            map.push_back(newnode);

        }
        root=map[0];
    }
    if(op==Option::MultiChar) {
        std::vector<node> map;
        int exists[text.size()];
        for(int i=0;i<text.size();i++){
            exists[i]=0;
        }
        for (int i = 0; i < text.size() - 1; i++) {
            std::string tmpp = text.substr(i, 2);
            if (map.size() == 0) {
                node tmp;
                tmp.data = tmpp;
                tmp.weight = 1;
                map.push_back(tmp);
            }//初始size为空的情况下
            else {
                for (int j = 0; j < map.size(); j++) {
                    if (map[j].data.compare(tmpp) == 0) {
                        map[j].weight++;
                        existed = true;
                    }
                }
                if (existed == true) {
                    existed = false;
                } else {
                    node tmp;
                    tmp.data = tmpp;
                    tmp.weight = 1;
                    map.push_back(tmp);
                }
            }
        }
        int least1 = -1, least2 = -1, least3 = -1;
        int pos1, pos2, pos3;
        std::string data1, data2, data3;
        if(map.size()!=0) {
            for (int j = 0; j < map.size(); j++) {
                if (map[j].weight >= least1) {
                    if (map[j].weight == least1 && data1.compare(map[j].data) > 0) {
                        data1 = map[j].data;
                        least1 = map[j].weight;
                        pos1 = j;
                    }
                    if (map[j].weight > least1) {
                        data1 = map[j].data;
                        least1 = map[j].weight;
                        pos1 = j;
                    }
                }
            }
        }
        if(map.size()!=0) {
            for (int j = 0; j < map.size() - 1; j++) {
                if (map[j].weight >= least2) {
                    if (map[j].weight == least2 && data2.compare(map[j].data) > 0 && j != pos1) {
                        data2 = map[j].data;
                        least2 = map[j].weight;
                        pos2 = j;
                    }
                    if (map[j].weight > least2 && j != pos2 && j != pos1) {
                        data2 = map[j].data;
                        least2 = map[j].weight;
                        pos2 = j;
                    }
                }
            }
        }
        if(map.size()!=0) {
            for (int j = 0; j < map.size(); j++) {
                if (map[j].weight >= least3) {
                    if (map[j].weight == least3 && data3.compare(map[j].data) > 0 && (j != pos2) && (j != pos1)) {
                        data3 = map[j].data;
                        least3 = map[j].weight;
                        pos3 = j;
                    }
                    if (map[j].weight > least3 && j != pos2 && j != pos1) {
                        data3 = map[j].data;
                        least3 = map[j].weight;
                        pos3 = j;
                    }
                }
            }
            pos1=pos2=pos3=0;
            for(int i=0;i<text.size()-1;i++){
                std::string tmp=text.substr(i,2);
                if(tmp.compare(data1)==0){
                    pos1++;
                    exists[i]=true;
                    exists[i+1]=true;
                    i++;
                }
                if(tmp.compare(data2)==0){
                    pos2++;
                    exists[i]=true;
                    exists[i+1]=true;
                    i++;
                }
                if(tmp.compare(data3)==0){
                    pos3++;
                    exists[i]=true;
                    exists[i+1]=true;
                    i++;
                }
            }
            node a;
            a.weight=pos1;
            a.data=data1;
            map2.push_back(a);
            a.weight=pos2;
            a.data=data2;
            map2.push_back(a);
            a.weight=pos3;
            a.data=data3;
            map2.push_back(a);
            for (int i = 0; i < text.size(); i++) {
                if (exists[i] == 0) {
                    if (map2.size() == 0) {
                        node tmp;
                        tmp.data = text[i];
                        tmp.weight = 1;
                        map2.push_back(tmp);
                    }//初始size为空的情况下
                    else {
                        for (int j = 0; j < map2.size(); j++) {
                            if (text[i] == map2[j].data[0] && map2[j].data.size() == 1) {
                                map2[j].weight++;
                                existed = true;
                            }
                        }
                        if (existed == true) {
                            existed = false;
                        } else {
                            node tmp;
                            tmp.data = text[i];
                            tmp.weight = 1;
                            map2.push_back(tmp);
                        }
                    }
                }
            }
        }
        while(map2.size()!=1){
            int weight1=99999;
            int weight2=99999;
            std::string data1,data2;
            int pos1,pos2;
            for(int i=0;i<map2.size();i++){
                if(map2[i].weight<=weight1){
                    if(map2[i].weight==weight1&&data1.compare(findleast(&map2[i]))>0){
                        pos1=i;
                        data1=findleast(&map2[i]);
                    }
                    if(map2[i].weight<weight1){
                        pos1=i;
                        data1=findleast(&map2[i]);
                        weight1=map2[i].weight;
                    }
                }
            }
            for(int i=0;i<map2.size();i++){
                if(map2[i].weight<=weight2){
                    if(map2[i].weight==weight2&&data2.compare(findleast(&map2[i]))>0&&i!=pos1){
                        pos2=i;
                        data2=findleast(&map2[i]);
                    }
                    if(map2[i].weight<weight2&&i!=pos1){
                        pos2=i;
                        data2=findleast(&map2[i]);
                        weight2=map2[i].weight;
                    }
                }
            }
            node* left=new node;
            left->weight=map2[pos1].weight;
            left->data=map2[pos1].data;
            left->left=map2[pos1].left;
            left->right=map2[pos1].right;
            node* right=new node;
            right->weight=map2[pos2].weight;
            right->data=map2[pos2].data;
            right->left=map2[pos2].left;
            right->right=map2[pos2].right;
            map2.erase(map2.begin()+pos1);
            if(pos1<pos2){
                pos2--;
            }
            map2.erase(map2.begin()+pos2);
            node newnode;
            newnode.left=left;
            newnode.right=right;
            newnode.weight=left->weight+right->weight;
            map2.push_back(newnode);
        }
        root=map2[0];
    }
    // TODO: Your code here
}

std::map<std::string, std::string> hfTree::getCodingTable()
{
    std::map<std::string ,std::string >k;
    findnode(&root,"",k);
    return k;
    // TODO: Your code here
    return std::map<std::string, std::string>();
}

std::vector<hfTree::node> hfTree::multiselect(std::string text,int* exists){
    std::vector<node> map;
    bool existed=false;
    for (int i = 0; i < text.size() - 1; i++) {
        if(exists[i]==1||exists[i+1]==1){
            continue;
        }
        std::string tmpp = text.substr(i, 2);
        if (map.size() == 0) {
            node tmp;
            tmp.data = tmpp;
            tmp.weight = 1;
            map.push_back(tmp);
        }//初始size为空的情况下
        else {
            for (int j = 0; j < map.size(); j++) {
                if (map[j].data.compare(tmpp) == 0) {
                    map[j].weight++;
                    existed = true;
                }
            }
            if (existed == true) {
                existed = false;
            } else {
                node tmp;
                tmp.data = tmpp;
                tmp.weight = 1;
                map.push_back(tmp);
            }
        }
    }
    return map;
}

void hfTree::findnode(node *a,std::string sm,std::map<std::string, std::string>& k){
    if((a->right== nullptr&&a->left== nullptr)&&a->data.compare("")!=0){
        k.insert(std::pair<std::string,std::string>(a->data,sm));
        return;
    }
    if(a->left!= nullptr){
        findnode(a->left,sm+"0",k);}
    if(a->right!= nullptr){
        findnode(a->right,sm+"1",k);}
}
std::string hfTree::findleast(node *st) {
    if(st->left== nullptr&&st->right!= nullptr){
        return findleast(st->right);
    }
    if(st->right== nullptr&&st->left!= nullptr){
        return findleast(st->left);
    }
    if(st->left== nullptr&&st->right== nullptr){
        return st->data;
    }
    if(findleast(st->right).compare(findleast(st->left))<0){
        return findleast(st->right);
    }
    else{
        return findleast(st->left);
    }
}