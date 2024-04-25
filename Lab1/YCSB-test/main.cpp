#include "radix_tree.hpp"
#include "compressed_radix_tree.hpp"
#include "rbtree.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <util.hpp>
#include <chrono>
//#include "test.cpp"
std::vector<double> times;
double findMedian(const std::vector<double>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("Vector is empty, cannot find median.");
    }

    std::vector<double> sortedVec = vec;
    std::sort(sortedVec.begin(), sortedVec.end());

    size_t size = sortedVec.size();
    if (size % 2 == 0) {
        return (sortedVec[size / 2 - 1] + sortedVec[size / 2]) / 2.0;
    } else {
        return sortedVec[size / 2];
    }
}
double findninety(const std::vector<double>&vec){
    if (vec.empty()) {
        throw std::invalid_argument("Vector is empty, cannot find median.");
    }

    std::vector<double> sortedVec = vec;
    std::sort(sortedVec.begin(), sortedVec.end());

    size_t size = sortedVec.size();
    return sortedVec[size/10*9];
}
double findninety_nine(const std::vector<double>&vec){
    if (vec.empty()) {
        throw std::invalid_argument("Vector is empty, cannot find median.");
    }

    std::vector<double> sortedVec = vec;
    std::sort(sortedVec.begin(), sortedVec.end());

    size_t size = sortedVec.size();
    return sortedVec[size/100*99];
}
double average_time(const std::vector<double>&vec){
    double sum=0;
    for(int i=0;i<vec.size();i++){
         sum+=vec[i];
    }
    return (sum/double(vec.size()))  ;
}
void testTime(int type,TreeType treeType)
{

    Tree *tree;
    switch(treeType){
        case TreeType::CompressedRadixTree: tree=new class CompressedRadixTree;
        break;

        case TreeType::RadixTree: tree=new class RadixTree;
        break;

        case TreeType::RBTree: tree=new class RBTree;
        break;
    }
    //auto start;
    switch(type){
        case 1: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<int> nums;
            // 创建一个均匀分布的整数生成器，范围从0到1（包含0，不包含1）
            for(int i=0;i<1000;i++){
                std::uniform_int_distribution<> dis(0, 1);
                nums.push_back(dis(gen));
            }
           // int random_number = dis(gen);
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++) {
                int s = zipf();
                if (nums[i]) {
                    tree->insert(s);
                } else { tree->find(s); }
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            times.push_back(diff.count());
            break;
        }
        case 2: {
            auto start2 = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++) {
                int s = zipf();
                tree->find(s);
            }
            auto end2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff2 = end2 - start2;
            times.push_back(diff2.count());
            break;
        }
        case 3:{
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<int> nums;
            // 创建一个均匀分布的整数生成器，范围从0到1（包含0，不包含1）
            for(int i=0;i<1000;i++){
                std::uniform_int_distribution<> dis(0, 3);
                nums.push_back(dis(gen));
            }
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < 1000; i++) {
                int s = zipf();
                if (nums[i]==0||nums[i]==1) {
                    tree->find(s);
                }
                if(nums[i]==2){ tree->insert(s); }
                if(nums[i]==3){tree->remove(s);}
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            times.push_back(diff.count());
            break;
        }
    }

    delete tree;
    return;
}

int main(int argc, char **argv)
{
    auto first=std::chrono::high_resolution_clock::now();
    int worktype;
    int treetype;
    std::cout<<"Please enter work type: 1: work type 1 , 2:work type 2 , 3: work type 3 --- "<<std::endl;
    std::cin>>worktype;
//    std::cout<<std::endl;
    std::cout<<"Please enter treetype: 1: Compressed Tree , 2: Radix tree2 , 3: Red Black tree --- "<<std::endl;
    std::cin>>treetype;
    switch (treetype) {
        case 1:for(int o=0;o<10000;o++ ) {
                testTime(worktype, CompressedRadixTree);
            }
            break;
        case 2:for(int o=0;o<10000;o++ ) {
                testTime(worktype, RadixTree);
            }
            break;
        case 3:for(int o=0;o<10000;o++ ){
                testTime(worktype, RBTree);
        }
        break;
        default: std::cout<<"wrong treetype!!!"<<std::endl;
    }

    std::cout<<"half"<<" "<<findMedian(times)<<std::endl;
    std::cout<<"ninety_percent"<<" "<<findninety(times)<<std::endl;
    std::cout<<"ninety_nine percent"<<" "<<findninety_nine(times)<<std::endl;
    std::cout<<"average_"<<" "<<average_time(times)<<std::endl;
    return 0;
}