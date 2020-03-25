#include <iterator>
#include <iostream>

template <class T>
class Allocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;

    pointer allocate(size_type count)
    {
        return static_cast<pointer>(::operator new(sizeof(value_type) * count));
    }

    void deallocate(pointer p)
    {
        ::operator delete(p);
    }

    void construct(pointer ptr, const T& val)
    {
        ::new(ptr) value_type(val);
    }

    void construct(pointer ptr)
    {
        ::new(ptr) value_type();
    }

    void destroy(pointer ptr)
    {
        ptr->~value_type();
    }
};


template<class T>
class Iterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
    using reference = T&;

private:
    T* ptr_;

public:
    explicit Iterator(T* ptr) : ptr_(ptr)
    {
    }

    bool operator== (const Iterator& other) const
    {
        return ptr_ == other.ptr_;
    }

    bool operator!= (const Iterator& other) const
    {
        return !(ptr_ == other.ptr_);
    }

    reference operator*() const
    {
        return *ptr_;
    }

    Iterator& operator++()
    {
        ptr_++;
        return *this;
    }

    Iterator& operator--()
    {
        ptr_--;
        return *this;
    }

    Iterator &operator++(int notused)
    {
        Iterator tmp(ptr_);
        ptr_++;
        return tmp;
    }

    Iterator &operator--(int notused)
    {
        Iterator tmp(ptr_);
        ptr_--;
        return tmp;
    }

    Iterator& operator+= (size_t n)
    {
        return *this += n;
    }

    Iterator& operator-= (size_t n)
    {
        return *this += -n;
    }

    Iterator operator+ (size_t n) const
    {
        return Iterator(ptr_ + n);
    }

    Iterator operator- (size_t n) const
    {
        return Iterator(ptr_ - n);
    }


    bool operator< (const Iterator& other) const
    {
        return ptr_ < other.ptr_;
    }

    bool operator> (const Iterator& other) const
    {
        return ptr_ > other.ptr_;
    }

    bool operator>= (const Iterator& other) const
    {
        return ptr_ >= other.ptr_;
    }

    bool operator<= (const Iterator& other) const
    {
        return ptr_ <= other.ptr_;
    }

    T& operator[] (size_t n)
    {
        return ptr_[n];
    }
};

template <class T, class Alloc = Allocator<T>>
class Vector {
public:
    using iterator = Iterator<T>;
    using reverse_iterator = std::reverse_iterator<Iterator<T>>;
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    using reference = T&;
private:
    Alloc allocate_;
    pointer data_;
    size_type size_;
    size_type capacity_ = 8;
public:
    explicit Vector(size_type count = 0) : size_(count)
    {
        if (count > 0) {
            capacity_ = count;
        }
        data_ = allocate_.allocate(capacity_);
        for (size_type i = 0; i < size_; ++i) {
            allocate_.construct(data_ + i);
        }
    }

    bool empty() const
    {
        return (size_ == 0);
    }
    size_type size() const
    {
        return size_;
    }
    size_type capacity() const
    {
        return capacity_;
    }

    iterator begin() const noexcept
    {
        return iterator(data_);
    }

    iterator end() const noexcept
    {
        return iterator(data_ + size_);
    }

    reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

    void clear() noexcept
    {
        resize(0);
    }

    ~Vector()
    {
        for (int i = 0; i < size_; ++i) {
            data_[i].~value_type();
        }
        allocate_.deallocate(data_);
    }

    reference operator[] (size_type n) {
        if (n >= size_)
            throw std::out_of_range("Out of range");
        return data_[n];
    }
    const reference operator[] (size_type n) const {
        if (n >= size_)
            throw std::out_of_range("Out of range");
        return data_[n];
    }

    void push_back(const value_type& value) {
        if (size_ == capacity_)
            reserve(capacity_ * 2);
        data_[size_] = value;
        ++size_;
    }

    void pop_back()
    {
        if (size_ > 0) {
            data_[size_].~value_type();
            size_--;
        }
    }

    void reserve(size_type n)
    {
        if (n < capacity_) {
            return;
        }
        pointer new_data = allocate_.allocate(n);
        for (size_type i = 0; i < size_; ++i) {
            allocate_.construct(new_data + i, *(data_ + i));
            allocate_.destroy(data_ + i);
        }
        allocate_.deallocate(data_);
        data_ = new_data;
        capacity_ = n;

    }

    void resize(size_type n)
    {
        if (capacity_ < n) {
            reserve(n);
        }
        for (size_type i = size_; i < n; i++) {
            allocate_.construct(data_ + i);
        }
        for (size_type i = n; i < size_; ++i) {
            allocate_.destroy(data_ + i);
        }
        size_ = n;
    }
};

int main()
{
    Vector <int> a;
    for (int i = 0; i < 10; i++)
    {
        a.push_back(i);
    }

    for (auto i = a.begin(); i < a.end(); ++i) {
        std::cout << *i <<std:: endl;
    }
}