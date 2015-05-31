#pragma once

#include "thread_safe_queue.h"

#include <cassert>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <utility>

using std::function;
using std::future;
using std::pair;
using std::promise;
using std::thread;
using std::vector;

template <typename R>
class TaskQueue {
public:
	using TaskFunction = function<R()>;
	using AsyncResult = future<R>;

	TaskQueue(const size_t max_number_of_tasks = 20,
		        const size_t number_of_workers = GetDefaultNumberOfWorkers());

	AsyncResult Submit(const TaskFunction new_task);

	AsyncResult TrySubmit(const TaskFunction new_task);

	std::vector<R> ShutDown() {
		tasks_.ShutDown();
		std::vector<R> result;
  	for (auto& res : thread_results_) {
  		std::vector<R> one_thread_result = res.get();
  		for (auto element : one_thread_result) {
  			result.push_back(element);
  		}
  	}
  	return result;
	}

	~TaskQueue();

	std::vector<R> Worker();

private:
	TaskQueue(const TaskQueue&) = delete;
	void operator = (const TaskQueue&) = delete;

	vector<thread> workers_;
	vector<future<std::vector<R>>> thread_results_;

	ThreadSafeQueue<std::packaged_task<R()>> tasks_;

	static size_t GetDefaultNumberOfWorkers() {
		int system_concurrency = thread::hardware_concurrency();
		return system_concurrency ? system_concurrency : 4;
	}


};

#include "task_queue.hpp"
