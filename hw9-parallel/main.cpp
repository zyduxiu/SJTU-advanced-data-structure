#include <future>
#include <iostream>
#include <chrono>
#include <vector>
unsigned  fib(unsigned n) {
    if( n <= 1 ) return n;
    return fib(n - 1) + fib(n - 2);
}
unsigned  fib_threads(unsigned n, unsigned threads = 0) {
    if (n <= 1)
        return n;
    if (threads) {
        unsigned half_threads = threads >> 1u;
        std::future<unsigned> fib2 = std::async(fib_threads, n - 2, half_threads - !(threads & unsigned (0x1)));
        unsigned fib1 = fib_threads(n - 1, half_threads);
        return fib1 + fib2.get();
    }
    return fib(n - 1) + fib(n - 2);
}
int main() {
    std::vector<double> vec;
    for(int i=0;i<100;i++) {
        auto start = std::chrono::high_resolution_clock::now();
        fib_threads(40, 64);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        vec.push_back(diff.count());
    }
    std::cout<<fib_threads(40, 4)<<std::endl;
    double avg;
    double sum;
    for(int i=0;i<100;i++){
        sum+=vec[i];
    }
    avg=sum/(100.0);
    std::cout<<avg;
}
