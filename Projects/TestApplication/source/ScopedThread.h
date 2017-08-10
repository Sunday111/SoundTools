#pragma once

#include <functional>
#include <mutex>

class ScopedThread
{
public:
	ScopedThread(std::function<void(bool&, std::mutex&)>&& fn) :
		m_finished(false),
		m_thread([&]()
	{
		fn(m_finished, m_mutex);
	})
	{}

	~ScopedThread()
	{
		{
			std::lock_guard<std::mutex> guard(m_mutex);
			m_finished = true;
		}

		m_thread.join();
	}

private:
	bool m_finished;
	std::mutex m_mutex;
	std::thread m_thread;
};