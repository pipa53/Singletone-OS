#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

template <typename T>
struct CreateUsingNew {
    static T* Create() {
        return new T;
    }
    static void Destroy(T* p) {
        delete p;
    }
};

template <typename T>
struct CreateUsingShared {
    static std::shared_ptr<T> Create() {
        return std::make_shared<T>();
    }
   
    static void Destroy(std::shared_ptr<T>) {
        
    }
};

struct NoLock {
    void lock() {}
    void unlock() {}
};

struct MutexLock {
    void lock() { m.lock(); }
    void unlock() { m.unlock(); }
private:
    std::mutex m;
};

template <
    typename T,
    template <typename> class CreationPolicy = CreateUsingNew,
    typename LockPolicy = MutexLock>
class Singleton {
public:
    static T& Instance() {
        std::lock_guard<LockPolicy> guard(l);
        if (!instance) {
            instance = CreationPolicy<T>::Create();
        }
        return *instance;
    }

    static void Destroy() {
        std::lock_guard<LockPolicy> guard(l);
        if (instance) {
            CreationPolicy<T>::Destroy(instance);
            instance = nullptr;
        }
    }

protected:
    static inline T* instance = nullptr;
    static inline LockPolicy l;

    Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

class Logger {
public:
    void SetOutputFile(const std::string& filename) {
        std::lock_guard<std::mutex> guard(lock);
        if (ofs.is_open())
            ofs.close();
        ofs.open(filename, std::ios::app);
    }

    void Log(const std::string& msg) {
        std::lock_guard<std::mutex> guard(lock);
        if (ofs.is_open())
            ofs << msg << '\n';
        else
            std::cout << msg << '\n';
    }

    ~Logger() {
        std::lock_guard<std::mutex> guard(lock);
        if (ofs.is_open())
            ofs.close();
    }

private:
    Logger() = default;

    std::ofstream ofs;
    std::mutex lock;

    friend struct CreateUsingNew<Logger>;
};

using AppLogger = Singleton<Logger, CreateUsingNew, MutexLock>;

int main() {
    auto& log = AppLogger::Instance();
    log.SetOutputFile("log.txt");
    log.Log("Запуск приложения");
    log.Log("Логируем в файл");
    AppLogger::Destroy();
}
