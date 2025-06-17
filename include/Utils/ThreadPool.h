/**
 * @file ThreadPool.h
 * @brief Simple thread pool implementation for concurrent task execution
 */

#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <deque>

/**
 * @class ThreadPool
 * @brief Simple thread pool for running tasks concurrently
 * @details Provides a fixed-size pool of worker threads that can execute
 * submitted tasks concurrently. Uses a task queue with condition variables
 * for efficient thread synchronization. Automatically manages thread lifecycle
 * and ensures proper cleanup on destruction.
 */
class ThreadPool {
public:
  /**
   * @brief Constructor: creates num_threads worker threads
   * @param num_threads Number of worker threads to create in the pool
   * @details Initializes the thread pool with the specified number of worker threads.
   * Each thread waits for tasks to be posted and executes them as they become available.
   */
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

  /**
   * @brief Destructor: shuts down the pool and joins all threads
   * @details Signals all worker threads to stop, wakes them up, and waits
   * for all threads to finish their current tasks before destruction.
   * Ensures no memory leaks or dangling threads.
   */
  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      running_ = false; // Signal shutdown
    }
    cond_.notify_all(); // Wake up all threads
    for (auto& t : threads_)
      if (t.joinable()) t.join(); // Wait for all threads to finish
  }

  /**
   * @brief Post a new task to the pool for execution
   * @param f Function object to execute (moved for efficiency)
   * @details Adds the task to the queue and notifies one worker thread.
   * The task will be executed by the next available worker thread.
   */
  void post(std::function<void()> f) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      tasks_.push_back(std::move(f)); // Add the task to the queue
    }
    cond_.notify_one(); // Wake up one worker thread
  }

private:
  std::vector<std::thread> threads_;              ///< Pool of worker threads
  std::deque<std::function<void()>> tasks_;       ///< Task queue
  std::mutex mutex_;                              ///< Protects access to the queue
  std::condition_variable cond_;                  ///< Used to notify worker threads
  bool running_;                                  ///< Indicates if the pool is running
};
