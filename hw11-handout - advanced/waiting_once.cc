#include "waiting_once_api.hpp"


//void call_once_waiting(init_function f) {
//    // 使用原子操作检查是否已初始化
//    if (!initialized_.load(std::memory_order_acquire)) {
//        // 加锁，确保只有一个线程能够执行初始化函数
//        std::unique_lock<std::mutex> lock(mtx_);
//        // 再次检查是否已初始化，以防止在获取锁之前其他线程已完成初始化
//        if (!initialized_.load(std::memory_order_relaxed)) {
//            // 执行初始化函数
//            f();
//            // 设置初始化标志，确保后续线程不会再次尝试初始化
//            initialized_.store(true, std::memory_order_release);
//        }
//    }
//}
