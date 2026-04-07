#pragma once

#include <mutex>

template <typename T>
class ConcurrentQueue {
public:
    ConcurrentQueue()
        : head_(new Node{T{}, nullptr}), tail_(head_), size_(0) {
    }

    void enqueue(const T& x) {
        std::lock_guard<std::mutex> lock(mtx_);

        Node* newNode = new Node{x, nullptr};
        tail_->next = newNode;
        tail_ = newNode;
        size_++;
    }

    bool dequeue(T* ret) {
        std::lock_guard<std::mutex> lock(mtx_);

        if (head_->next == nullptr) {
            return false;
        }

        Node* temp = head_->next;
        *ret = temp->data;
        head_->next = temp->next;

        if (temp == tail_) {
            tail_ = head_;
        }

        delete temp;
        size_--;
        return true;
    }

    int size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return size_;
    }

    ~ConcurrentQueue() {
        while (head_ != nullptr) {
            Node* temp = head_->next;
            delete head_;
            head_ = temp;
        }
    }

private:
    struct Node {
        T data;
        Node* next;
    };

    Node* head_;
    Node* tail_;
    int size_;
    mutable std::mutex mtx_;
};