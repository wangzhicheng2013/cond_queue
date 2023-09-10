#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <chrono>
template <class T>
class cond_queue {
private:
    unsigned int size_ = 16;
    int block_timeout_ = 2;      // block for 2 seconds
    std::deque<T>             queue_;
    std::mutex                mutex_;
    std::condition_variable   cond_;
public:
    inline void set_size(unsigned int sz) {
        size_ = sz;
    }
    inline void set_block_timeout(int t) {
        block_timeout_ = t;
    }
    void push(const T &e) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.size() >= size_) {
            lock.unlock();
            return;
        }
        queue_.push_back(e);
        lock.unlock();
        cond_.notify_all();
    }
    bool try_pop(T &e) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!queue_.empty()) {
            e = queue_.front();
            queue_.pop_front();
            return true;
        }
        return false;
    }
    bool pop(T &e) {
        std::unique_lock<std::mutex> lock(mutex_);
        // wait for timeout
        cond_.wait_for(lock, std::chrono::seconds(block_timeout_), [this] { return queue_.size() > 0; } );
        if (false == queue_.empty()) {
            e = queue_.front();
            queue_.pop_front(); 
            return true;
        }
        return false;
    }
};