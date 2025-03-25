#pragma once

#include <array>
#include <cassert>
#include <initializer_list>
#include <memory>


/// <summary>
/// Template for a bounded queue. Implementation via two pointers {head, tail} on std::array.
/// Head points to the front elemet, tail to the back. Pushing an element to front/back 
/// advances the respective pointer in a cyclic manner (0 -> n-1; n-1 -> 0).
/// If the queue is full, element insertion will overwrite the last element of opposite side.
/// </summary>
/// <typeparam name="T">type to be stored</typeparam>
/// <typeparam name="n">capacity</typeparam>
template <class T, size_t n>
class BoundedQueue
{
public:
	BoundedQueue() {}
	BoundedQueue(std::initializer_list<T> l) {
		const int imax = std::min(l.size(), n);
		auto l_ptr = l.begin();
		for (int i = 0; i < imax; i++) {
			data[i] = *l_ptr;
			l_ptr++;
		}
		m_size = imax;
		m_tail = (imax - 1 + n) % n;
	}
	bool empty() const { return m_size==0; }
	T& front() {
		assert(!empty());
		return data[m_head];
	}
	const T& front() const {
		assert(!empty());
		return data[m_head];
	}
	T& back() {
		assert(!empty());
		return data[m_tail];
	}
	const T& back() const {
		assert(!empty());
		return data[m_tail];
	}
	int size() const { return m_size; }
	int capacity() const { return n; }
	void clear () {
		m_head = 0;
		m_tail = n - 1;
		m_size = 0;
	}

	void push_front(const T& elem) {
		if (m_size == n) m_tail = (m_tail - 1 + n) % n;
		else m_size++;
		m_head = (m_head - 1 + n) % n;
		data[m_head] = elem;
	}

	void push_back(const T& elem) {
		if (m_size == n) m_head = (m_head + 1) % n;
		else m_size++;
		m_tail = (m_tail + 1) % n;
		data[m_tail] = elem;
	}

	void pop_front(const T& elem) {
		if (empty()) return;
		m_size--;
		m_head = (m_head + 1) % n;
	}

	void pop_back(const T& elem) {
		if (empty()) return;
		m_size--;
		m_tail = (m_tail - 1 + n) % n;
	}

	T& operator[](int i) { return data[(m_head + i) % n]; }
	const T& operator[](int i) const { return data[(m_head + i) % n]; }

private:
	int m_head = 0;
	int m_tail = n - 1;
	int m_size = 0;
	std::array<T, n> data;
};


/// <summary>
/// Template for a bounded queue with capacity at runtime. Implementation via two pointers {head, tail} on carray.
/// Head points to the front elemet, tail to the back. Pushing an element to front/back 
/// advances the respective pointer in a cyclic manner (0 -> capacity-1; capacity-1 -> 0).
/// If the queue is full, element insertion will overwrite the last element of opposite side.
/// </summary>
/// <typeparam name="T">type for queue elements</typeparam>
template <class T>
class BoundedQueueHeap
{
public:
	BoundedQueueHeap(int capacity) : m_head(0), m_tail(capacity - 1),
		m_size(0), m_capacity(capacity), m_data(std::make_unique<T[]>(capacity)) {}
	BoundedQueueHeap(std::initializer_list<T> l,int capacity) : m_head(0), m_tail(capacity - 1),
		m_size(l.size()), m_capacity(capacity), m_data(std::make_unique<T[]>(capacity))
	{
		const int imax = std::min(static_cast<int>(l.size()), m_capacity);
		auto l_ptr = l.begin();
		for (int i = 0; i < imax; i++) {
			m_data[i] = *l_ptr;
			l_ptr++;
		}
		m_size = imax;
		m_tail = (imax - 1 + m_capacity) % m_capacity;
	}
	bool empty() const { return m_size == 0; }
	T& front() {
		assert(!empty());
		return m_data[m_head];
	}
	const T& front() const {
		assert(!empty());
		return m_data[m_head];
	}
	T& back() {
		assert(!empty());
		return m_data[m_tail];
	}
	const T& back() const {
		assert(!empty());
		return m_data[m_tail];
	}
	int size() const { return m_size; }
	int capacity() const { return m_capacity; }
	void clear() {
		m_head = 0;
		m_tail = m_capacity - 1;
		m_size = 0;
	}

	void push_front(const T& elem) {
		if (m_size == m_capacity) m_tail = (m_tail - 1 + m_capacity) % m_capacity;
		else m_size++;
		m_head = (m_head - 1 + m_capacity) % m_capacity;
		m_data[m_head] = elem;
	}

	void push_back(const T& elem) {
		if (m_size == m_capacity) m_head = (m_head + 1) % m_capacity;
		else m_size++;
		m_tail = (m_tail + 1) % m_capacity;
		m_data[m_tail] = elem;
	}

	void pop_front(const T& elem) {
		if (empty()) return;
		m_size--;
		m_head = (m_head + 1) % m_capacity;
	}

	void pop_back(const T& elem) {
		if (empty()) return;
		m_size--;
		m_tail = (m_tail - 1 + m_capacity) % m_capacity;
	}

	T& operator[](int i) { return m_data[(m_head + i) % m_capacity]; }
	const T& operator[](int i) const { return m_data[(m_head + i) % m_capacity]; }
private:
	int m_head;
	int m_tail;
	int m_size;
	int m_capacity;
	std::unique_ptr<T[]> m_data;
};