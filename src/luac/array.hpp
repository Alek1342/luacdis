#pragma once
#include "lasm/output_stream.hpp"
#include "log.hpp"
#include "luac/input_stream.hpp"
#include "types.hpp"

namespace Luac {
    class Prototype;
    template <typename T>
    class Array {
    private:
        i32 m_length = 0;
        T* m_items = nullptr;

    public:
        void init(i32 length) {
            m_length = length;

            if (length > 0) {
                T* items = new T[length];
                if (!items) {
                    LOGF("Luac::Array<T>.init malloc failed");
                }

                m_items = items;
            }
        }

        T& operator[](size_t index) {
            return m_items[index];
        }

        const T& operator[](size_t index) const {
            return m_items[index];
        }

        T* items() const {
            return m_items;
        }

        size_t length() const {
            return m_length;
        }

        ~Array() {
            if (m_items) {
                delete[] m_items;
            }
        }

        void load(Luac::InputStream* stream) {
            T::loadArray(stream, this);
        }

        void print(Lasm::OutputStream* stream) {
            T::printArray(stream, this);
        }
    };
    template <>
    void Array<Prototype>::init(i32 length);
    template <>
    Array<Prototype>::~Array();

} // namespace Luac