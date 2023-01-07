#pragma once

#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include "array_ptr.h"

class ReserveProxyObj {
public:
    ReserveProxyObj() = delete;

    explicit ReserveProxyObj(size_t new_capacity) 
        :capacity(new_capacity)
    {
    }
    size_t GetCapacity() {
        return capacity;
    }
private:
    size_t capacity;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size)
        :size_(size), 
        capacity_(size)
    {
        ArrayPtr<Type> tmp(size);
        items_.swap(tmp);
        std::fill(items_.Get(), items_.Get() + size_, 0);
        tmp.Release();
    }

    SimpleVector(size_t size, const Type& value)
        :size_(size), capacity_(size)
    {
        ArrayPtr<Type> tmp(size);
        items_.swap(tmp);
        std::fill(items_.Get(), items_.Get() + size_, value);
        tmp.Release();
    }

    SimpleVector(std::initializer_list<Type> init) 
        :items_(init.size()),
        size_(init.size()),
        capacity_(init.size())
    {
        std::move(init.begin(), init.end(), items_.Get());
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {

        return size_ == 0 ? true : false;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    Type& At(size_t index) {
        if (index < size_) {
            return items_[index];
        }
        else {
            throw std::out_of_range("no index");
        }
    }

     const Type& At(size_t index) const {
        if (index < size_) {
            return items_[index];
        }
        else {
            throw std::out_of_range("no index");
        }
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        }
        else if (new_size <= capacity_) {
            for (size_t i = size_; i < new_size; i++) {
                items_[i] = std::move(Type{});;
            }
            size_ = new_size;
        }
        else {
            ArrayPtr<Type> temp(new_size);

            std::move(items_.Get(), items_.Get() + size_, temp.Get());
            for (size_t i = size_; i < new_size; ++i) {
                temp[i] = std::move(Type{});
            }

            items_.swap(temp);
            size_ = new_size;
            capacity_ = 2*new_size;
        }
    }

    Iterator begin() noexcept {
        return items_.Get();
    }

    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return ConstIterator(items_.Get());
    }

    ConstIterator end() const noexcept {
        return ConstIterator(items_.Get() + size_);
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

    SimpleVector(const SimpleVector& other) 
        :size_(other.size_),
        capacity_(other.capacity_)
    {
        ArrayPtr<Type> new_arr(size_);

        for (size_t i = 0; i < size_; i++)
        {
            new_arr[i] = other.items_[i];
        }

        items_.swap(new_arr);
        new_arr.Release();

    }

    SimpleVector(SimpleVector&& other)
        :items_(other.size_),
        size_(std::move(other.size_)),
        capacity_(std::move(other.capacity_)) {

        items_.swap(other.items_);
        other.Clear(); 

    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty())
                Clear();
            else {
                SimpleVector rhs_copy(rhs);
                SimpleVector::swap(rhs_copy);
            }
        }
                return *this;
            
    }

    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }
        items_[size_] = item;
        ++size_;
    }

    void PushBack(Type&& item) {

        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t dist_to_pos = std::distance(cbegin(), pos);
        size_t old_size = size_; 

        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }

        size_ = old_size;
        auto it = begin() + dist_to_pos;
        std::copy_backward(it, end(), end() + 1);
        items_[dist_to_pos] = std::move(value);
        ++size_;
        return Iterator(begin() + dist_to_pos);

    }

    Iterator Insert(ConstIterator pos,Type&& value) {
        assert(pos >= begin() && pos <= end());
        auto dist_to_pos = std::distance(cbegin(), pos); 
        size_t old_size = size_; 

        if (size_ == capacity_) {
            size_ != 0 ? ChangeCapacity(2 * size_) : ChangeCapacity(1);
        }
        size_ = old_size;
        auto it = begin() + dist_to_pos;
        std::move_backward(it, end(), end() + 1);
        items_[dist_to_pos] = std::move(value);
        ++size_;
        return Iterator(begin() + dist_to_pos);

    }

    void PopBack() noexcept {
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        Iterator new_pos = const_cast<Iterator>(pos);
        std::move(std::next(const_cast<Iterator>(pos)), this->end(), const_cast<Iterator>(pos));
        --size_;
        return new_pos;
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void swap(SimpleVector&& other) noexcept {
        items_.swap(other.items_);
        size_ = std::move(other.size_);
        capacity_ = std::move(other.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (capacity_ < new_capacity) {
            ChangeCapacity(new_capacity);
        }
    }

    SimpleVector(ReserveProxyObj new_capacity) {
        if (new_capacity.GetCapacity() > capacity_)
        {
            ArrayPtr<Type> new_arr(new_capacity.GetCapacity());
            std::copy(items_.Get(), items_.Get() + size_, new_arr.Get());
            std::fill(new_arr.Get() + size_, new_arr.Get() + new_capacity.GetCapacity(), 0);
            items_.swap(new_arr);
            new_arr.Release();
            size_ = size_;
            capacity_ = new_capacity.GetCapacity();
        }
    }

    bool operator==(const SimpleVector& other);

    bool operator!=(const SimpleVector& other);

    bool operator<(const SimpleVector& other);

    bool operator<=(const SimpleVector& other);

    bool operator>=(const SimpleVector& other);

    bool operator>(const SimpleVector& other);

    private:
        ArrayPtr<Type> items_;

        size_t size_ = 0;
        size_t capacity_ = 0;

        void ChangeCapacity(const size_t new_size) {
            SimpleVector new_arr(new_size);
            size_t prev_size = size_;
            std::move(begin(), end(), new_arr.begin());
            swap(new_arr);
            size_ = prev_size;
        }
};

template<typename Type>
inline bool SimpleVector<Type>::operator==(const SimpleVector& other)
{
    if (size_ != other.size_) {
        return false;
    }
    else {
        return std::equal(cbegin(), cend(), other.cbegin(), other.cend());
    }
}

template<typename Type>
inline bool SimpleVector<Type>::operator!=(const SimpleVector& other)
{
    return !(*this == other);
}

template<typename Type>
inline bool SimpleVector<Type>::operator<(const SimpleVector& other)
{
    return std::lexicographical_compare(cbegin(), cend(), other.cbegin(), other.cend());
}

template<typename Type>
inline bool SimpleVector<Type>::operator<=(const SimpleVector& other)
{
    return ((*this < other) || (*this == other)) ? true : false;
}

template<typename Type>
inline bool SimpleVector<Type>::operator > (const SimpleVector& other)
{
    return !(*this <= other);
}

template<typename Type>
inline bool SimpleVector<Type>::operator>=(const SimpleVector& other)
{
    return !(*this < other);
}





