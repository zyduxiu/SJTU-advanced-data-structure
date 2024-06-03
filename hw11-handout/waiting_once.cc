#include "waiting_once_api.hpp"



void waiting_once::call_once_waiting(init_function f)
{
    std::unique_lock<std::mutex> lock(mutex);
    if (!initialized)
    {
        initialized = true;
        f();
    }
	// TODO: implement this
}
