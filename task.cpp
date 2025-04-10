#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

template <typename T>
struct CreateUsingNew
{
	static T *Create()
	{
		return new T
	}
	static void Destroy(T *p)
	{
		delete p
	}
}

template <typename T>
struct CreateUsingShared
{
	static std::shared_ptr<T> Create()
	{
		return std::make_shared<T>()
	}
}

// Threading policies

struct NoLock
{
	void lock() {}
	void unlock() {}
}

struct MutexLock
{
	void lock()
	{
		m.lock()
	}
	void unlock()
	{
		m.unlock()
	}
private:
	std::mutex m
}

// Singleton template

template <
	typename T,
	template <typename> class CreationPolicy,
	typename LockPolicy>
class Singleton
{
public:
	static T &Instance()
	{
		l.lock()
		if (!instance)
			instance = CreationPolicy<T>::Create()
		l.unlock()
		return *instance
	}

	static void Destroy()
	{
		l.lock()
		if (instance)
		{
			CreationPolicy<T>::Destroy(instance)
			instance = nullptr
		}
		l.unlock()
	}
private:
	static inline T *instance = nullptr
	static inline LockPolicy l
	Singleton() = delete
	Singleton(const Singleton &) = delete
	Singleton &operator=(const Singleton &) = delete
}

// Пример класса для Singleton

class Logger
{
public:
	void SetOutputFile(const std::string &filename)
	{
		l.lock()
		if (ofs.is_open())
			ofs.close()
		ofs.open(filename, std::ios::app)
		l.unlock()
	}

	void Log(const std::string &msg)
	{
		l.lock()
		if (ofs.is_open())
			ofs << msg << '\n'
		else
			std::cout << msg << '\n'
		l.unlock()
	}

	~Logger()
	{
		l.lock()
		if (ofs.is_open())
			ofs.close()
		l.unlock()
	}
private:
	Logger() = default
	std::ofstream ofs
	std::mutex l
	friend struct CreateUsingNew<Logger>
}

using AppLogger = Singleton<Logger, CreateUsingNew, MutexLock>

int main()
{
	auto &log = AppLogger::Instance()
	log.SetOutputFile("log.txt")
	log.Log("Starting application")
	log.Log("Logging to file")
	AppLogger::Destroy()
}
