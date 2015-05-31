
#include <iostream>

template <typename R>
std::vector<R> TaskQueue<R>::Worker() {
	std::vector<R> all_results;
  std::packaged_task<R()> task;
	while (tasks_.Dequeue(task)) {
		std::future<R> result = task.get_future();
	  task();
	  all_results.push_back(result.get());
  }
  return all_results;
}

template <typename R>
TaskQueue<R>::TaskQueue(const size_t max_number_of_tasks,
						            const size_t number_of_workers)
	: tasks_(max_number_of_tasks) {
	for (size_t i = 0; i < number_of_workers; ++i) {
		std::packaged_task<std::vector<R>()> task([this]() {
			std::vector<R> all_results;
  		std::cout << "in worker\n";
  		std::packaged_task<R()> task;
			while (tasks_.Dequeue(task)) {
				std::cout << "after dequeue\n";
				task.reset();
				std::future<R> future_for_result = task.get_future();
				std::cout << "after get_future\n";
				task();
				R result = future_for_result.get();
				std::cout << "after get result\n";
	  		std::cout << std::this_thread::get_id() << " " << result.get_source_path() << "\n";
	  	  all_results.push_back(result);
  		}
  		return all_results;
		});
		thread_results_.push_back(task.get_future());
		workers_.emplace_back(std::move(task));
	}
}

template <typename R>
future<R> TaskQueue<R>::Submit(const function<R()> new_task) {
  std::packaged_task<R()> task(new_task);
	future<R> task_result = task.get_future();
	tasks_.Enqueue(std::move(task));
	//std::cout << "submit\n";
	return task_result;
}

template <typename R>
future<R> TaskQueue<R>::TrySubmit(const function<R()> new_task) {
  std::packaged_task<R()> task(new_task);
  future<R> task_result = task.get_future();
	if (tasks_.TryEnqueue(std::move(task))) {
    return task_result;
	}
	else {
		return future<R>();
	}
}

template <typename R>
TaskQueue<R>::~TaskQueue() {
	tasks_.ShutDown();
	for (thread& one_worker : workers_) {
		assert(one_worker.joinable());
		one_worker.join();
	}
}

/*template <typename R>
std::vector<R> TaskQueue<R>::Worker() {
	std::vector<R> all_results;
  std::packaged_task<R()> task;
	while (tasks_.Dequeue(task)) {
		std::future<R> result = task.get_future();
	  task();
	  all_results.push_back(result.get());
  }
  return all_results;
}*/

/*template <typename R>
void TaskQueue<R>::ActiveWait(future<R>& async_result) {
  std::packaged_task<R()> task;
	std::chrono::milliseconds span(0);
	while (async_result.wait_for(span) != std::future_status::ready) {
		if (!tasks_.TryDequeue(task)) {
			std::this_thread::yield();
			continue;
		}
    task();
	}
}*/
