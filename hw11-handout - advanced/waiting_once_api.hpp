#ifndef WAITING_ONCE_HW_HPP
#define WAITING_ONCE_HW_HPP

#include <atomic>
#include <mutex>
#include <functional>

#if __cplusplus < 201100
#  error "C++11 or better is required"
#endif
#ifdef __has_include
# if __has_include(<version>)
#   include <version>
# endif
#endif

struct waiting_once
{
#if __cpp_lib_move_only_function >= 202110L
	using init_function = std::move_only_function<void()>;
#else
	using init_function = std::function<void()>;
#endif
	void call_once_waiting(init_function f){
        {
            if (!initialized_.load(std::memory_order_acquire)) {
                std::unique_lock<std::mutex> lock(mtx_);
                if (!initialized_.load(std::memory_order_relaxed)) {
                    f();
                    initialized_.store(true, std::memory_order_release);
                }
            }
        }
    }

private:
    std::atomic<bool> initialized_{false};
    std::mutex mtx_;
	// TODO: implement this
};

#endif // WAITING_ONCE_HW_HPP
