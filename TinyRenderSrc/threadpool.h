#pragma once 
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>


	using Task = std::function<void()>;

	class ThreadPool {
	
	public:
		explicit ThreadPool(const int& number);
		~ThreadPool();
		void Start();
		void Close();
		template<typename T,typename... Args>
		void AddTask(T&& func, Args&&... args) {
			if (m_is_close.load() == true) {
				return;
			}
			auto new_task = std::bind(std::forward<T>(func), std::forward<Args>(args)...);
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_task_queue.emplace(std::move(new_task));
			}
			m_cond.notify_one();
		}

		template<typename T,typename... Args>
		auto AddRetTask(T&& func, Args&&... args) -> std::future<std::result_of_t<T(Args...)>> {
			if (m_is_close.load() == true) {
				return std::future<std::result_of_t<T(Args...)>>{};
			}
			using ReturnType = std::result_of_t<T(Args...)>;
			auto bound_func = std::bind(std::forward<T>(func), std::forward<Args>(args)...);
			auto package_task = std::make_shared<std::packaged_task<ReturnType()>>(std::move(bound_func));

			std::future<ReturnType> returnVal = package_task->get_future();

			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_task_queue.emplace([package_task]() {
					(*package_task)();
					});
			}
			m_cond.notify_one();
			return returnVal;
		}

	private:
		void AddThread();
		

	private:
		std::vector<std::unique_ptr<std::thread>> m_thread_array;
		std::queue<Task> m_task_queue;
		
		std::condition_variable m_cond;
		std::mutex m_mutex;
		

		std::atomic<int> thread_number;
		std::atomic<bool> m_is_close;

	};


