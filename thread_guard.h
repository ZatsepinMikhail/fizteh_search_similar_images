#ifndef H_THREAD_GUARD
#define H_THREAD_GUARD

#include <thread>

using std::thread;

class ThreadGuard {
public:
	ThreadGuard(thread& given_thread) : inner_thread_(given_thread) {}
	ThreadGuard(ThreadGuard&& other_thread_guard)
		: inner_thread_(other_thread_guard.inner_thread_) {}
	~ThreadGuard() {
		if (inner_thread_.joinable()) {
			inner_thread_.join();
		}
	}
private:
	ThreadGuard(const ThreadGuard&) = delete;
	void operator = (const ThreadGuard&) = delete;

	thread& inner_thread_;
};

#endif