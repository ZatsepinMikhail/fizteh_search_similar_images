#pragma once

#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <queue>

#include "mutex_guard.h"

using std::condition_variable;
using std::exception;
using std::iostream;
using std::mutex;
using std::queue;
using std::unique_lock;

class QueueShutDownException : public exception {
public:
	virtual const char* what() const throw() {
		return "The queue was shutted down";
	}
};

template<typename T>
class ThreadSafeQueue {
public:
	ThreadSafeQueue(size_t capacity)
		: continue_working_(true), queue_capacity_(capacity) {
	}
	void Enqueue(T new_element) {
		unique_lock<mutex> lock(queue_mutex_);
		while (queue_.size() == queue_capacity_ && continue_working_) {
			queue_not_full_cond_.wait(lock);
		}
		if (!continue_working_) {
			throw QueueShutDownException();
		}
		queue_.push(move(new_element));
		queue_not_empty_cond_.notify_one();
	}

	bool TryEnqueue(T new_element) {
		unique_lock<mutex> lock(queue_mutex_);
		if (queue_.size() == queue_capacity_) {
			return false;
		} else if (!continue_working_) {
			throw QueueShutDownException();
		} else {
			queue_.push(move(new_element));
			queue_not_empty_cond_.notify_one();
			return true;
		}
	}

	bool Dequeue(T& popped_element) {
		unique_lock<mutex> lock(queue_mutex_);
		while (queue_.empty() && continue_working_) {
			queue_not_empty_cond_.wait(lock);
		}
		if (queue_.empty()) {
			return false;
		}
		std::cout << "in Dequeue : " << queue_.size() << "\n";
		popped_element = std::move(queue_.front());
		queue_.pop();
		queue_not_full_cond_.notify_one();
		return true;
	}

	void ShutDown() {
		unique_lock<mutex> lock(queue_mutex_);
		continue_working_ = false;
		queue_not_empty_cond_.notify_all();
		queue_not_full_cond_.notify_all();
	}

private:
	ThreadSafeQueue(const ThreadSafeQueue&) = delete;
	void operator = (const ThreadSafeQueue&) = delete;

	queue<T> queue_;
	mutex queue_mutex_;
	condition_variable queue_not_empty_cond_;
	condition_variable queue_not_full_cond_;
	bool continue_working_;
	size_t queue_capacity_;
};
