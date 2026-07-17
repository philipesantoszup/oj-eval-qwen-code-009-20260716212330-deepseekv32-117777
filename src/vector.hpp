#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring>
#include <new>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    static T* allocate(size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    static void deallocate(T* ptr) {
        if (ptr) {
            ::operator delete(ptr);
        }
    }
    
    void construct_at(T* pos, const T& value) {
        new (pos) T(value);
    }
    
    void destroy_at(T* pos) {
        pos->~T();
    }
    
    void resize_capacity(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        
        T* new_data = allocate(new_capacity);
        for (size_t i = 0; i < size_; ++i) {
            construct_at(new_data + i, data_[i]);
            destroy_at(data_ + i);
        }
        deallocate(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
	class const_iterator;
	class iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		T* ptr_;
        const vector* parent_;

	public:
        iterator(T* ptr = nullptr, const vector* parent = nullptr) : ptr_(ptr), parent_(parent) {}
        
		iterator operator+(const int &n) const
		{
			return iterator(ptr_ + n, parent_);
		}
		iterator operator-(const int &n) const
		{
			return iterator(ptr_ - n, parent_);
		}
        
		int operator-(const iterator &rhs) const
		{
			if (parent_ != rhs.parent_) {
                throw invalid_iterator();
            }
            return ptr_ - rhs.ptr_;
		}
        
		iterator& operator+=(const int &n)
		{
			ptr_ += n;
            return *this;
		}
        
		iterator& operator-=(const int &n)
		{
			ptr_ -= n;
            return *this;
		}
        
		iterator operator++(int) {
            iterator temp = *this;
            ++ptr_;
            return temp;
        }
        
		iterator& operator++() {
            ++ptr_;
            return *this;
        }
        
		iterator operator--(int) {
            iterator temp = *this;
            --ptr_;
            return temp;
        }
        
		iterator& operator--() {
            --ptr_;
            return *this;
        }
        
		T& operator*() const {
            return *ptr_;
        }
        
		bool operator==(const iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        
		bool operator==(const const_iterator &rhs) const {
            return ptr_ == rhs.get_ptr();
        }
        
		bool operator!=(const iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }
        
		bool operator!=(const const_iterator &rhs) const {
            return ptr_ != rhs.get_ptr();
        }
        
        T* get_ptr() const { return ptr_; }
        const vector* get_parent() const { return parent_; }
	};
	
	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		const T* ptr_;
        const vector* parent_;

	public:
        const_iterator(const T* ptr = nullptr, const vector* parent = nullptr) : ptr_(ptr), parent_(parent) {}
        const_iterator(const iterator& other) : ptr_(other.get_ptr()), parent_(other.get_parent()) {}
        
        const_iterator operator+(const int &n) const {
            return const_iterator(ptr_ + n, parent_);
        }
        
        const_iterator operator-(const int &n) const {
            return const_iterator(ptr_ - n, parent_);
        }
        
        int operator-(const const_iterator &rhs) const {
            if (parent_ != rhs.parent_) {
                throw invalid_iterator();
            }
            return ptr_ - rhs.ptr_;
        }
        
        const_iterator& operator+=(const int &n) {
            ptr_ += n;
            return *this;
        }
        
        const_iterator& operator-=(const int &n) {
            ptr_ -= n;
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ++ptr_;
            return temp;
        }
        
        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }
        
        const_iterator operator--(int) {
            const_iterator temp = *this;
            --ptr_;
            return temp;
        }
        
        const_iterator& operator--() {
            --ptr_;
            return *this;
        }
        
        const T& operator*() const {
            return *ptr_;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr_ == rhs.ptr_;
        }
        
        bool operator==(const iterator &rhs) const {
            return ptr_ == rhs.get_ptr();
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr_ != rhs.ptr_;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr_ != rhs.get_ptr();
        }
        
        const T* get_ptr() const { return ptr_; }
        const vector* get_parent() const { return parent_; }
	};

	vector() : data_(nullptr), size_(0), capacity_(0) {}
    
	vector(const vector &other) : data_(nullptr), size_(0), capacity_(0) {
        if (other.size_ > 0) {
            data_ = allocate(other.size_);
            capacity_ = other.size_;
            size_ = other.size_;
            for (size_t i = 0; i < size_; ++i) {
                construct_at(data_ + i, other.data_[i]);
            }
        }
    }
    
	~vector() {
        clear();
        deallocate(data_);
    }
    
	vector &operator=(const vector &other) {
        if (this == &other) return *this;
        
        clear();
        
        if (other.size_ > capacity_) {
            deallocate(data_);
            data_ = allocate(other.size_);
            capacity_ = other.size_;
        }
        
        size_ = other.size_;
        for (size_t i = 0; i < size_; ++i) {
            construct_at(data_ + i, other.data_[i]);
        }
        
        return *this;
    }
    
	T & at(const size_t &pos) {
        if (pos >= size_) {
            throw index_out_of_bound();
        }
        return data_[pos];
    }
    
	const T & at(const size_t &pos) const {
        if (pos >= size_) {
            throw index_out_of_bound();
        }
        return data_[pos];
    }
    
	T & operator[](const size_t &pos) {
        return at(pos);
    }
    
	const T & operator[](const size_t &pos) const {
        return at(pos);
    }
    
	const T & front() const {
        if (size_ == 0) {
            throw container_is_empty();
        }
        return data_[0];
    }
    
	const T & back() const {
        if (size_ == 0) {
            throw container_is_empty();
        }
        return data_[size_ - 1];
    }
    
	iterator begin() {
        return iterator(data_, this);
    }
    
	const_iterator begin() const {
        return const_iterator(data_, this);
    }
    
	const_iterator cbegin() const {
        return begin();
    }
    
	iterator end() {
        return iterator(data_ + size_, this);
    }
    
	const_iterator end() const {
        return const_iterator(data_ + size_, this);
    }
    
	const_iterator cend() const {
        return end();
    }
    
	bool empty() const {
        return size_ == 0;
    }
    
	size_t size() const {
        return size_;
    }
    
	void clear() {
        for (size_t i = 0; i < size_; ++i) {
            destroy_at(data_ + i);
        }
        size_ = 0;
    }
    
	iterator insert(iterator pos, const T &value) {
        if (pos.get_parent() != this) {
            throw invalid_iterator();
        }
        
        size_t index = pos.get_ptr() - data_;
        
        if (size_ + 1 > capacity_) {
            resize_capacity(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        
        for (size_t i = size_; i > index; --i) {
            construct_at(data_ + i, data_[i - 1]);
            destroy_at(data_ + (i - 1));
        }
        
        construct_at(data_ + index, value);
        ++size_;
        
        return iterator(data_ + index, this);
    }
    
	iterator insert(const size_t &ind, const T &value) {
        if (ind > size_) {
            throw index_out_of_bound();
        }
        return insert(iterator(data_ + ind, this), value);
    }
    
	iterator erase(iterator pos) {
        if (pos.get_parent() != this) {
            throw invalid_iterator();
        }
        if (size_ == 0) {
            throw container_is_empty();
        }
        
        size_t index = pos.get_ptr() - data_;
        if (index >= size_) {
            throw index_out_of_bound();
        }
        
        destroy_at(data_ + index);
        
        for (size_t i = index; i < size_ - 1; ++i) {
            construct_at(data_ + i, data_[i + 1]);
            destroy_at(data_ + (i + 1));
        }
        
        --size_;
        
        if (index == size_) {
            return end();
        } else {
            return iterator(data_ + index, this);
        }
    }
    
	iterator erase(const size_t &ind) {
        if (ind >= size_) {
            throw index_out_of_bound();
        }
        return erase(iterator(data_ + ind, this));
    }
    
	void push_back(const T &value) {
        insert(end(), value);
    }
    
	void pop_back() {
        if (size_ == 0) {
            throw container_is_empty();
        }
        erase(size_ - 1);
    }
};


}

#endif