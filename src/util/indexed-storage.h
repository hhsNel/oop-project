#pragma once
#define INDEXED_STORAGE_H

#include <vector>
#include <cstddef>
#include <cstdint>

namespace util {
	template<typename T>
	class indexed_storage {
	public:
		typedef std::uint32_t id_t;
	protected:
		id_t size;
		std::vector<T> objects;
		std::vector<id_t> lookup;
		std::vector<size_t> reverse_lookup;
	public:
		indexed_storage();
		id_t const size() const;
		id_t const add(T const& object);
		void remove(id_t const id);
		T &operator[](id_t id) const;

		struct entry {
			id_t id;
			T& value;
		};
		struct const_entry {
			id_t id;
			const T& value;
		};
		class iterator;
		class const_iterator;
		friend class iterator;
		friend class const_iterator;

		class iterator {
		private:
			indexed_storage *m_storage;
			size_t m_index;

			iterator(indexed_storage *st, size_t idx);

			friend class indexed_storage<T>;
		public:
			iterator() = default;

			entry operator*() const;
			iterator &operator++();
			iterator operator++(int);
			iterator &operator--();
			iterator operator--(int);
			bool operator==(iterator const& other) const;
			bool operator!=(iterator const& other) const;
		}
		class const_iterator {
		private:
			indexed_storage const *m_storage;
			size_t m_index;

			iterator(indexed_storage const *st, size_t idx);

			friend class indexed_storage<T>;
		public:
			iterator() = default;

			const_entry operator*() const;
			const_iterator &operator++();
			const_iterator operator++(int);
			const_iterator &operator--();
			const_iterator operator--(int);
			bool operator==(const_iterator const& other) const;
			bool operator!=(const_iterator const& other) const;
		}

		iterator begin() const;
		iterator end() const;
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
		void erase(iterator const it);
		void erase(const_iterator const it);
	};
}

