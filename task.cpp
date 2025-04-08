#include <iostream>
#include <memory>
#include <mutex>

// Базовый класс для синхронизации
class SyncStrategy {
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual ~SyncStrategy() = default;
};

// Стратегия синхронизации с использованием std::mutex
class MutexSync : public SyncStrategy {
private:
    std::mutex mtx;
public:
    void lock() override {
        mtx.lock();
    }
    
    void unlock() override {
        mtx.unlock();
    }
};

// Класс Singleton
template <typename T, typename Sync = MutexSync>
class Singleton {
private:
    static std::unique_ptr<T> instance;
    static Sync sync;

    Singleton() = default;

public:
    // Удаляем конструктор копирования и оператор присваивания
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& getInstance() {
        sync.lock();
        if (!instance) {
            instance.reset(new T());
        }
        sync.unlock();
        return *instance;
    }
};

// Инициализация статических членов
template <typename T, typename Sync>
std::unique_ptr<T> Singleton<T, Sync>::instance = nullptr;

template <typename T, typename Sync>
Sync Singleton<T, Sync>::sync;

// Пример класса, который будет использоваться как Singleton
class MyClass {
public:
    void doSomething() {
        std::cout << "Doing something!" << std::endl;
    }
};

int main() {
    // Получение экземпляра Singleton и использование его
    MyClass& myInstance = Singleton<MyClass>::getInstance();
    myInstance.doSomething();

    return 0;
}
