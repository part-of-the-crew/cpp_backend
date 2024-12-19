#pragma once

#include <cstddef>
#include <utility>
#include <iterator>
#include <cassert>

template <typename T>
class SingleLinkedList {

    struct Node {
        Node() = default;
        Node(const T& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        T value{};
        Node* next_node = nullptr;
    };

public:
    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<T> values) {
        for (auto it = values.end(); it != values.begin(); ) {
            --it; // Move backward
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList tmp;
        auto it = tmp.before_begin();
        for (const auto& value : other) {
            it = tmp.InsertAfter(it, value);
        }
        swap(tmp);
    }
    /*
    SingleLinkedList(SingleLinkedList&& other)
        : head_(std::move(other.head_))
        , size_(std::move(other.size_))
    {}
    */
    SingleLinkedList& operator=(SingleLinkedList rhs) noexcept {
        if (this != &rhs){
            swap(rhs);
        }
        return *this;
    }

   
    void swap(SingleLinkedList& other) noexcept {
        if (this != &other) {
            std::swap(head_.next_node, other.head_.next_node);
            std::swap(size_, other.size_);
        }
    }

    void PushFront(const T& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    size_t GetSize() const noexcept {
        return size_;
    }
    void Clear() noexcept {
        while (head_.next_node){
            auto e = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete e;
            size_--;
        }
    }
    ~SingleLinkedList() {
        Clear();
        head_.next_node = nullptr;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    size_t GetSize() noexcept {
        return size_;
    }

    bool IsEmpty() noexcept {
        return size_ == 0;
    }


    
    template <typename ValueType>
    class BasicIterator {

    private:
        friend class SingleLinkedList;
        //explicit BasicIterator(Node* node): node_(node) {};
        explicit BasicIterator(const Node* node): node_(const_cast<Node*>(node)) {}
    public:
        using value_type = ValueType;
        using difference_type = std::ptrdiff_t;
        using pointer = const ValueType*;
        using reference = const ValueType&;
        using iterator_category = std::forward_iterator_tag;

        Node* node_ = nullptr;
        /*
        BasicIterator& operator= (const BasicIterator<const Type>& rhs){
            if (this != &rhs) {
                auto rhs_copy(rhs);
                node_ = rhs_copy.node_;
                rhs_copy.node_ = nullptr;
            }
            return *this;
        };
        */
        BasicIterator& operator= (BasicIterator<T>& rhs){
            if (this != &rhs) {
                auto rhs_copy(rhs);
                node_ = rhs_copy.node_;
                rhs_copy.node_ = nullptr;
            }
            return *this;
        };
        
        template <typename OtherValueType>
        BasicIterator& operator=(const BasicIterator<OtherValueType>& rhs) {
            if (this != reinterpret_cast<const BasicIterator*>(&rhs)) {
                node_ = rhs.node_;
            }
            return *this;
        }
        BasicIterator( const BasicIterator<T>& rhs ): node_(rhs.node_){};
        BasicIterator( const BasicIterator<const T>& rhs ): node_(rhs.node_){};

        explicit BasicIterator() = default;
        using Iterator = BasicIterator<T>;
        using ConstIterator = BasicIterator<const T>;
        [[nodiscard]] bool operator==(const BasicIterator<const T>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<const T>& rhs) const noexcept {
            return node_!= rhs.node_;
        }
        [[nodiscard]] bool operator==(const BasicIterator<T>& rhs) const noexcept {
            return node_ == rhs.node_;
        }
        [[nodiscard]] bool operator!=(const BasicIterator<T>& rhs) const noexcept {
            return node_!= rhs.node_;
        }

        [[nodiscard]] T& operator*() noexcept {
            assert(node_!= nullptr);
            return node_->value;
        }
        [[nodiscard]] T* operator->() noexcept {
            assert(node_!= nullptr);
            return &node_->value;
        }
        [[nodiscard]] T& operator*() const noexcept {
            assert(node_!= nullptr);
            return node_->value;
        }
        [[nodiscard]] T* operator->() const noexcept {
            assert(node_!= nullptr);
            return &node_->value;
        }

        BasicIterator& operator++() noexcept {
            assert(node_!= nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            assert(node_!= nullptr);
            auto old_value(*this);
            ++(*this);
            return old_value;
        }
    };

    using Iterator = BasicIterator<T>;
    using ConstIterator = BasicIterator<const T>;
    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }
    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }
    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{nullptr};
    }
    
    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{&head_};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{&head_};
    }

    Iterator InsertAfter(ConstIterator pos, const T& value) {
        assert(pos.node_!= nullptr);
        auto new_node{new Node(value, pos.node_->next_node) };
        pos.node_->next_node = new_node;
        size_++;
        return Iterator{new_node};
    }

    void PopFront() noexcept {
        if (head_.next_node) {
            delete std::exchange(head_.next_node, head_.next_node->next_node);
            --size_;
        }
    }


    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_!= nullptr);
        if (pos.node_->next_node) {
            auto next_node = pos.node_->next_node->next_node;
            delete pos.node_->next_node;
            pos.node_->next_node = next_node;
            --size_;
        }
        return Iterator{pos.node_->next_node};
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename T>
void swap(SingleLinkedList<T>& lhs, SingleLinkedList<T>& rhs) noexcept {
    if (lhs != rhs){
        lhs.swap(rhs);
    }
}

template <typename T>
bool operator==(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()){
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
bool operator<=(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    return !(rhs < lhs);
}

template <typename T>
bool operator>(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    return rhs < lhs;
}

template <typename T>
bool operator>=(const SingleLinkedList<T>& lhs, const SingleLinkedList<T>& rhs) {
    return !(lhs < rhs);
}
