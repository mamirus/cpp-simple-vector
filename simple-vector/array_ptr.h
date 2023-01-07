#pragma once
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size > 0) {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&& other) {
        raw_ptr_.swap(move(other.raw_ptr_));
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&& other) {
        raw_ptr_.swap(move(other.raw_ptr_));
    }

    Type* Release() noexcept {
        Type* release_raw_ptr_ = raw_ptr_;
        raw_ptr_ = nullptr;
        return release_raw_ptr_;
    }

    Type& operator[](size_t index) noexcept {
        return (raw_ptr_[index]);
    }

    const Type& operator[](size_t index) const noexcept {
        return (raw_ptr_[index]);
    }

    explicit operator bool() const {
        return raw_ptr_ == nullptr ? false : true;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        if (other) {
            Type* tmp = raw_ptr_;
            raw_ptr_ = other.Get();
            other.raw_ptr_ = tmp;
        }
    }

    void swap(ArrayPtr&& other) noexcept {
        if (other) {
            Type* tmp = move(raw_ptr_);
            raw_ptr_ = move(other.Get());
            other.raw_ptr_ = move(tmp);
        }
    }

private:
    Type* raw_ptr_ = nullptr;
};