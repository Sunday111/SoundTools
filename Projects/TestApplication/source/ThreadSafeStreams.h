#pragma once

#include <istream>
#include <mutex>
#include <ostream>
#include <string>

class MutexWrapper
{
protected:
	template<typename F>
	void Execute(F&& f) const
	{
		std::lock_guard<std::mutex> guard(m_mutex);
		f();
	}

	mutable std::mutex m_mutex;
};

class ThreadSafeOStream : public MutexWrapper
{
public:
	ThreadSafeOStream(std::ostream& ostream) :
		m_ostream(ostream)
	{}

	template<typename T>
	std::ostream& operator<<(const T& val) const
	{
		Execute([&]()
		{
			m_ostream << val;
		});

		return m_ostream;
	}

private:
	std::ostream& m_ostream;
};

class ThreadSafeIStream : public MutexWrapper
{
public:
	ThreadSafeIStream(std::istream& istream) :
		m_istream(istream)
	{}

	template<typename T>
	std::istream& operator>>(T& val) const
	{
		Execute([&]()
		{
			m_istream >> val;
		});

		return m_istream;
	}

	void GetLine(std::string& string)
	{
		Execute([&]()
		{
			std::getline(m_istream, string);
		});
	}

private:
	std::istream& m_istream;
};