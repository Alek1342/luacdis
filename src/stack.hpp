#pragma once
#include "log.hpp"
#include <cstdlib>

template <typename T>
class Stack {
private:
    size_t m_capacity = 0;
    size_t m_length = 0;
    T* m_items = nullptr;

public:
    Stack(size_t capacity = 65536) {
        m_capacity = capacity;

        if (capacity > 0) {
            T* items = static_cast<T*>(calloc(capacity, sizeof(T)));
            if (!items) {
                LOGF("Stack malloc failed");
            }

            m_items = items;
        }
    }

    T* items() const {
        return m_items;
    }

    size_t length() const {
        return m_length;
    }

    void push(const T& value) {
        if (m_length >= m_capacity) {
            size_t new_capacity = m_capacity * 2;

            T* new_items = static_cast<T*>(realloc(m_items, new_capacity * sizeof(T)));

            if (!new_items) {
                LOGF("Stack realloc failed");
            }

            m_items = new_items;
            m_capacity = new_capacity;
        }

        m_items[m_length++] = value;
    }

    T pop() {
        if (m_length == 0) {
            LOGF("Stack underflow");
        }

        return m_items[--m_length];
    }

    ~Stack() {
        if (m_items) {
            free(m_items);
        }
    }
};
