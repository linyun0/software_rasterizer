#include "threadpool.h"



	ThreadPool::ThreadPool(const int& number) :thread_number(number), m_is_close(true){
		

	}
	ThreadPool::~ThreadPool() {
		Close();
	}
	void ThreadPool::Close() {
		if (m_is_close == false) {
			m_is_close.store(true);
			m_cond.notify_all();
		}
		for (const auto& item : m_thread_array) {
			if (item->joinable()) {
				item->join();
			}
		}
		m_thread_array.clear();
	}
	void ThreadPool::AddThread() {

		auto func = [this]() {

			while (true) {

				Task task;
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					m_cond.wait(lock, [this]()->bool {
						return !m_task_queue.empty() || m_is_close;
						});
					if (m_is_close.load() == true) {
						break;
					}
					task = m_task_queue.front();
					m_task_queue.pop();
				}
				task();
			}
		};
		

		auto single_thread = std::make_unique<std::thread>(func);
		m_thread_array.emplace_back(std::move(single_thread));

	}

	void ThreadPool::Start() {
		if (m_is_close == true) {
			m_is_close.store(false);
			for (int i = 0; i < thread_number.load(); ++i) {
				AddThread();
			}
			m_cond.notify_all();
		}
	}




