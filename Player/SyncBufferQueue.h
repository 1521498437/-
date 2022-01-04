/**
* @file SyncBufferQueue.h
* @brief 同步缓冲队列
* @authors yandaoyang
* @date 2020/12/08
* @note
*/

#pragma once
#include <list>
#include <mutex>
#include <atomic>
#include <condition_variable>

template<int MAX_SIZE, typename T>
class SyncBufferQueue
{
public:
	SyncBufferQueue() = default;

	~SyncBufferQueue() {
		stop();
		_queue.clear();
	}

	bool enqueue(const T& t, bool block = true) {
		return push(t, block);
	}

	bool enqueue(T&& t, bool block = true) {
		return push(std::forward<T>(t), block);
	}

	bool dequeue(T& t) {
		std::unique_lock<std::mutex> lck(_mutex);
		_cond_not_empty.wait(lck, [this] { 
			return _shutdown || _is_clear || notEmpty();
		});
		if (_shutdown) 
			return false;
		if (_is_clear)
		{
			_queue.clear();
			_is_clear = false;
			return false;
		}
		//改成右值引用后性能提升了1倍多！
		t = std::move(_queue.front());
		_queue.pop_front();
		_cond_not_full.notify_one();
		return true;
	}

	void clear() {
		{
			std::lock_guard<std::mutex> lck(_mutex);
			if (_is_clear) return;
			_is_clear = true;
		}
		_cond_not_full.notify_all();
		_cond_not_empty.notify_all();
	}

	void stop() {
		{
			std::lock_guard<std::mutex> lck(_mutex);
			if (_shutdown) return;
			_shutdown = true;
		}
		_cond_not_full.notify_all();
		_cond_not_empty.notify_all();
	}

	bool empty() {
		std::lock_guard<std::mutex> lck(_mutex);
		return !notEmpty();
	}

	bool full() {
		std::lock_guard<std::mutex> lck(_mutex);
		return !notFull();
	}

	size_t size() {
		std::lock_guard<std::mutex> lck(_mutex);
		return _queue.size();
	}

private:
	bool notEmpty() const {
		return !_queue.empty();
	}

	bool notFull() const {
		return _queue.size() < MAX_SIZE;
	}

	template<typename F>
	bool push(F&& f, bool block) {
		if (block) //阻塞调用
		{
			std::unique_lock<std::mutex> lck(_mutex);
			_cond_not_full.wait(lck, [this] {
				return _shutdown || _is_clear || notFull();
			});
			if (_shutdown)
				return false;
			if (_is_clear)
			{
				_queue.clear();
				_is_clear = false;
				return false;
			}
			_queue.emplace_back(std::forward<F>(f));
			_cond_not_empty.notify_one();
			return true;
		}
		else //非阻塞调用，丢弃队头
		{
			std::unique_lock<std::mutex> lck(_mutex);
			if (!notFull()) _queue.pop_front();
			_queue.emplace_back(std::forward<F>(f));
			_cond_not_empty.notify_one();
			return true;
		}
	}

private:
	std::list<T>            _queue;
	std::mutex              _mutex;
	std::condition_variable _cond_not_full;
	std::condition_variable _cond_not_empty;
	std::atomic_bool        _is_clear{ false };
	std::atomic_bool        _shutdown{ false };
};
