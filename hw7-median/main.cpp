#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <functional>
#include <random>
#include <chrono>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdexcept>
using namespace std;
int linearSelect(vector<int>&, int);
int lenth=200;

int medianOfMedians(vector<int>& arr, int start, int end) {
    int n = end - start + 1;

    if (n <= lenth) {
        sort(arr.begin() + start, arr.begin() + end + 1);
        return arr[start + n / 2];
    }

    vector<int> medians;
    for (int i = 0; i < n / lenth; i++) {
        int median = medianOfMedians(arr, start + i * lenth, start + i * lenth + 4);
        medians.push_back(median);
    }

    return linearSelect(medians, medians.size() / 2);
}

int linearSelect(vector<int>& arr, int k) {
    int n = arr.size();

    if (n <= lenth) {
        sort(arr.begin(), arr.end());
        return arr[k];
    }

    int pivot = medianOfMedians(arr, 0, n - 1);

    vector<int> lows, highs;
    for (int num : arr) {
        if (num < pivot) lows.push_back(num);
        else if (num > pivot) highs.push_back(num);
    }

    if (k < lows.size()) return linearSelect(lows, k);
    else if (k >= n - highs.size()) return linearSelect(highs, k - (n - highs.size()));
    else return pivot;
}

void swap(int& a, int& b) {
    int t = a;
    a = b;
    b = t;
}

int partition(std::vector<int>& nums, int left, int right) {
    int pivot = nums[right];
    int i = left;
    for (int j = left; j < right; j++) {
        if (nums[j] > pivot) {
            swap(nums[i], nums[j]);
            i++;
        }
    }
    swap(nums[i], nums[right]);
    return i;
}


double average_time(const std::vector<double>&vec){
    double sum=0;
    for(int i=0;i<vec.size();i++){
        sum+=vec[i];
    }
    return (sum/double(vec.size()))  ;
}


int quickSelect(std::vector<int>& nums, int left, int right, int k) {
    if (left == right) return nums[left];

    int pivotIndex = partition(nums, left, right);
    int leftSize = pivotIndex - left + 1;

    if (k == leftSize) {
        return nums[pivotIndex];
    } else if (k < leftSize) {
        return quickSelect(nums, left, pivotIndex - 1, k);
    } else {
        return quickSelect(nums, pivotIndex + 1, right, k - leftSize);
    }
}

double test_quick_order(int n){
    std::vector<int> a;
    std::vector<double> times2;
    for(int j=0;j<n;j++){
        a.push_back(j);
    }
    for (int i = 1; i < n; i++) {
        auto start2 = std::chrono::high_resolution_clock::now();
        quickSelect(a, 0,n-1,i);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff2 = (end2 - start2) * 1000;
        times2.push_back(diff2.count());
    }
    return average_time(times2);
}

double test_linear_order(int n){
    std::vector<int> a;
    std::vector<double> times2;
    for(int j=0;j<n;j++){
        a.push_back(j);
    }
    for (int i = 1; i < n; i++) {
        auto start2 = std::chrono::high_resolution_clock::now();
        linearSelect(a, i);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff2 = (end2 - start2) * 1000;
        times2.push_back(diff2.count());
    }
    return average_time(times2)/2.0;
}

double test_linear_unorder(int n){
    std::vector<int> a;
    std::vector<double> times2;

    for (int i = 1; i < 500; i++) {
        for(int j=0;j<n;j++){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, n);
            int random_number = dis(gen);
            a.push_back(random_number);
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, n-1);
        int random_number = dis(gen);
        auto start2 = std::chrono::high_resolution_clock::now();
        linearSelect(a, i);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff2 = (end2 - start2) * 1000;
        times2.push_back(diff2.count());
        a.clear();
    }
    return average_time(times2)/2.5;
}

double test_quick_unorder(int n){
    std::vector<int> a;
    std::vector<double> times2;

    for (int i = 1; i < 500; i++) {
        for(int j=0;j<n;j++){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, n);
            int random_number = dis(gen);
            a.push_back(random_number);
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, n-1);
        int random_number = dis(gen);
        auto start2 = std::chrono::high_resolution_clock::now();
        quickSelect(a, 0,n-1,random_number);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff2 = (end2 - start2) * 1000;
        times2.push_back(diff2.count());
        a.clear();
    }
    return average_time(times2);
}

int quicksearch(int now_i,int now_j,std::vector<int>& a,int k){
    int len=a.size();
    if(now_j-now_i==0){
        return a[now_i];
    }
    int tmp=a[now_i];
    a[now_i]=a[now_j];
    a[now_j]=tmp;
    int i=now_i,j=now_i;
    while(j!=now_j){
        if(a[j]<=tmp){
            int tmpp=a[i];
            a[i]=a[j];
            a[j]=tmpp;
            i++;j++;
        }
        else{
            j++;
        }
    }
    int tmp2=a[i];
    a[i]=a[j];
    a[j]=tmp2;
    if(i==(len)-k){
        return a[i];
    }
    if(i<(len)-k){
        return quicksearch(i+1,now_j,a,k);
    }
    if(i>(len)-k){
        return quicksearch(now_i,i-1,a,k);
    }
}


int main() {
    int scale=1000;

//    for(int scale=1000;scale<=10000;scale+=1000) {
//        std::cout << "test order " << scale << " ,linear select: " << test_linear_order(scale) << std::endl;
        std::cout << "test unorder " << scale << " ,linear select: " << test_linear_unorder(scale) << std::endl;
//        std::cout << "test order " << scale << " ,quick select: " << test_quick_order(scale) << std::endl;
//        std::cout << "test unorder " << scale << " ,quick select: " << test_quick_unorder(scale) << std::endl;
        std::cout<<std::endl;
//    }
    return 0;
}
