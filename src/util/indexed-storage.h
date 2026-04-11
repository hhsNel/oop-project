#pragma once
#define INDEXED_STORAGE_H

#include <vector>
#include <cstddef>

namespace util {
	template<typename T>
	class indexed_storage {
	public:
		typedef uint32_t id_t;
	protected:
		id_t size;
		std::vector<T> objects;
		std::vector<id_t> lookup;
		std::vector<size_t> reverse_lookup;
	public:
		using iterator = std::vector<T>::iterator;
		using const_iterator = std::vector<T>::const_iterator;

		indexed_storage();
		id_t size() const;
		id_t add(T const& object);
		void remove(id_t id);
		T &operator[](id_t id) const;

		iterator begin() const;
		iterator end() const;
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;
	};
}

