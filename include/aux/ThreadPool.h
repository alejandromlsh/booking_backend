#pragma once
#include <thread>
#include <vector>
#include <deque>
#include <functional>
#include <mutex>
#include <condition_variable>

// Simple thread pool for running tasks concurrently
class ThreadPool {
public:
    // Constructor: creates num_threads worker threads
    explicit ThreadPool(std::size_t num_threads)
        : running_(true)
    {
        for (std::size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        // Wait for a task or shutdown
                        cond_.wait(lock, [this] { return !running_ || !tasks_.empty(); });
                        if (!running_ && tasks_.empty())
                            return; // Exit thread if shutting down and no tasks left
                        task = std::move(tasks_.front());
                        tasks_.pop_front();
                    }
                    task(); // Execute the task
                }
            });
        }
    }

    // Destructor: shuts down the pool and joins all threads
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            running_ = false; // Signal shutdown
        }
        cond_.notify_all(); // Wake up all threads
        for (auto& t : threads_)
            if (t.joinable()) t.join(); // Wait for all threads to finish
    }

    // Post a new task to the pool
    void post(std::function<void()> f) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push_back(std::move(f)); // Add the task to the queue
        }
        cond_.notify_one(); // Wake up one worker thread
    }

private:
    std::vector<std::thread> threads_;                 // Pool of worker threads
    std::deque<std::function<void()>> tasks_;          // Task queue
    std::mutex mutex_;                                 // Protects access to the queue
    std::condition_variable cond_;                     // Used to notify worker threads
    bool running_;                                     // Indicates if the pool is running
};
