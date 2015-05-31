#ifndef H_MUTEX_GUARD
#define H_MUTEX_GUARD

#include <mutex>

using std::mutex;

class MutexGuard {
public:
	MutexGuard(mutex& arg_mutex) : inner_mutex_(arg_mutex) {
		inner_mutex_.lock();
	}
	MutexGuard(MutexGuard&& other_mutex_guard)
		: inner_mutex_(other_mutex_guard.inner_mutex_) {}
	~MutexGuard() {
		inner_mutex_.unlock();
	}
private:
	MutexGuard(const MutexGuard&) = delete;
	void operator = (const MutexGuard&) = delete;

	mutex& inner_mutex_;
};

#endif