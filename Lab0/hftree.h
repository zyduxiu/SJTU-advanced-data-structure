#ifndef HFTREE_H
#define HFTREE_H

#include <string>
#include <map>
#include<vector>
class hfTree
{
    struct node{
        int weight;
        std::string data;
        node *left;
        node *right;
    };
private:
    node root;
    // TODO: Your code here
public:
    enum class Option
    {
        SingleChar,
        MultiChar
    };
    std::vector<node> multiselect(std::string a,int *exists);
    hfTree(const std::string &text, const Option op);
    std::map<std::string, std::string> getCodingTable();
    std::string findleast(node* st);
    void findnode(node* a,std::string sm,std::map<std::string, std::string>& k);
};

#endif