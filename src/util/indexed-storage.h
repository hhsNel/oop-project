#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <utility>
#include <iterator>

namespace util {
    template<typename T>
    class indexed_storage {
    public:
        using id_t = std::uint32_t;

        static constexpr id_t nullid = 0;

        struct entry {
            id_t id;
            T& value;
        };
        struct const_entry {
            id_t id;
            const T& value;
        };

        class iterator {
        private:
            indexed_storage* m_storage;
            size_t m_index;
            iterator(indexed_storage* st, size_t idx) : m_storage(st), m_index(idx) {}
            friend class indexed_storage<T>;
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = entry;
            using difference_type   = std::ptrdiff_t;
            using pointer           = entry*;
            using reference         = entry;

            iterator() : m_storage(nullptr), m_index(0) {}

            entry operator*() const {
                return {m_storage->lookup[m_index], m_storage->objects[m_index]};
            }

            iterator& operator++() { ++m_index; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
            iterator& operator--() { --m_index; return *this; }
            iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

            bool operator==(const iterator& other) const { return m_index == other.m_index; }
            bool operator!=(const iterator& other) const { return m_index != other.m_index; }
        };

        class const_iterator {
        private:
            const indexed_storage* m_storage;
            size_t m_index;
            const_iterator(const indexed_storage* st, size_t idx) : m_storage(st), m_index(idx) {}
            friend class indexed_storage<T>;
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = const_entry;
            using difference_type   = std::ptrdiff_t;
            using pointer           = const_entry*;
            using reference         = const_entry;

            const_iterator() : m_storage(nullptr), m_index(0) {}

            const_entry operator*() const {
                return {m_storage->lookup[m_index], m_storage->objects[m_index]};
            }

            const_iterator& operator++() { ++m_index; return *this; }
            const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
            const_iterator& operator--() { --m_index; return *this; }
            const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

            bool operator==(const const_iterator& other) const { return m_index == other.m_index; }
            bool operator!=(const const_iterator& other) const { return m_index != other.m_index; }
        };

    protected:
        std::vector<T> objects;
        std::vector<id_t> lookup;
        std::vector<size_t> reverse;

        id_t m_next_id = 1;

    public:
        indexed_storage() {
            reverse.push_back(0);
        }

        id_t size() const { return static_cast<id_t>(objects.size()); }

        id_t add(const T& object) {
            id_t new_id = m_next_id++;

            if (new_id >= reverse.size()) {
                reverse.resize(new_id + 1, 0);
            }

            reverse[new_id] = objects.size();
            objects.push_back(object);
            lookup.push_back(new_id);

            return new_id;
        }

        void remove(id_t id) {
            if (id == nullid || id >= reverse.size() || reverse[id] == 0 && (objects.empty() || lookup[0] != id)) {
                return;
            }

            size_t index_to_remove = reverse[id];
            size_t last_index = objects.size() - 1;

            if (index_to_remove != last_index) {
                T& last_obj = objects.back();
                id_t last_id = lookup.back();

                objects[index_to_remove] = std::move(last_obj);
                lookup[index_to_remove] = last_id;

                reverse[last_id] = index_to_remove;
            }

            objects.pop_back();
            lookup.pop_back();
            reverse[id] = 0;
        }

        T& operator[](id_t id) {
            assert(id != nullid && id < reverse.size() && reverse[id] < objects.size());
            return objects[reverse[id]];
        }

        const T& operator[](id_t id) const {
            assert(id != nullid && id < reverse.size() && reverse[id] < objects.size());
            return objects[reverse[id]];
        }

        iterator begin() { return iterator(this, 0); }
        iterator end() { return iterator(this, objects.size()); }

        const_iterator begin() const { return const_iterator(this, 0); }
        const_iterator end() const { return const_iterator(this, objects.size()); }

        const_iterator cbegin() const { return const_iterator(this, 0); }
        const_iterator cend() const { return const_iterator(this, objects.size()); }

        iterator erase(iterator it) {
            if (it != end()) {
                remove((*it).id);
            }
            return it;
        }

        const_iterator erase(const_iterator it) {
            if (it != cend()) {
                const_cast<indexed_storage*>(this)->remove((*it).id);
            }
            return it;
        }

        bool contains(id_t id) const {
            return id != nullid && id < reverse.size() &&
                   (reverse[id] != 0 || (!objects.empty() && lookup[0] == id));
        }
    };
}
