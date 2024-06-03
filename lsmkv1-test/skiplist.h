#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
// #include <optional>
// #include <vector>
#include <string>
#include <list>
#include <iostream>
#include <functional>

namespace skiplist {
using key_type = uint64_t;
// using value_type = std::vector<char>;
using value_type = std::string;

   struct node{
        node **right;
        value_type data;
        key_type key;
        int lev;
        node(int level){
            right=new node*[level+1];
            for(int i=0;i<=level;i++){
                right[i]=nullptr;
                lev=level;
            }

        };
       ~node(){
           delete [] right;
       }
   };
class skiplist_type
{

	// add something here
public:
    double coin;
    int MAX_LEVEL;
    node *head;
	explicit skiplist_type(double p = 0.5);
	void put(key_type key, const value_type &val);
	//std::optional<value_type> get(key_type key) const;
	std::string get(key_type key) const;
    bool del(key_type key);
    void clear();
    std::list<std::pair<uint64_t, std::string>> scan(int key1,int key2);
	// for hw1 only
	int query_distance(key_type key) const;
    void print();
};

} // namespace skiplist

#endif // SKIPLIST_H
