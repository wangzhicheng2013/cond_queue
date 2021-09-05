#pragma once
#include <deque>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
template <class T>
class cond_queue {
public:
    cond_queue() : size_(1024) {
    }
    inline void set_size(int sz) {
        size_ = sz;
    }
    void push(const T &e) {
        boost::unique_lock<boost::mutex> lock(mutex_);
        if (queue_.size() >= size_) {
            lock.unlock();
            return;
        }
        queue_.push_back(e);
        lock.unlock();
        cond_.notify_one();
    }
    bool try_pop(T &e) {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (!queue_.empty()) {
            e = queue_.front();
            queue_.pop_front();
            return true;
        }
        return false;
    }
    void pop(T &e) {
        boost::unique_lock<boost::mutex> lock(mutex_);
        while (queue_.empty()) {
            cond_.wait(lock);
        }
        e = queue_.front();
        queue_.pop_front();
    }
    void flush() {
        boost::lock_guard<boost::mutex> lock(mutex_);
        cond_.notify_all();
    }
private:
    int size_;
    std::deque<T>               queue_;
    boost::mutex                mutex_;
    boost::condition_variable   cond_;
};