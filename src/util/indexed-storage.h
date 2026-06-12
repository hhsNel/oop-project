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
		/* element identifier type */
		typedef std::uint32_t id_t;

		/* sentinel identifier */
		static constexpr id_t nullid = 0;

		/* iterator entry */
		struct entry {
			id_t id;
			T& value;
		};
		/* const iterator entry */
		struct const_entry {
			id_t id;
			T const& value;
		};

		class iterator {
		private:
			/* bound indexed_storage */
			indexed_storage* storage;
			/* current index */
			size_t index;
			/* constructor */
			iterator(indexed_storage* st, size_t idx) : storage(st), index(idx) {}
			friend class indexed_storage<T>;
		public:
			/* c++ semantics */
			using iterator_category = std::forward_iterator_tag;
			/* c++ semantics */
			using value_type		= entry;
			/* c++ semantics */
			using difference_type   = std::ptrdiff_t;
			/* c++ semantics */
			using pointer		   = entry*;
			/* c++ semantics */
			using reference		 = entry;

			/* constructor */
			iterator() : storage(nullptr), index(0) {}

			/* access operator */
			entry operator*() const {
				return {storage->lookup[index], storage->objects[index]};
			}

			/* iteration */
			iterator& operator++() { ++index; return *this; }
			/* iteration */
			iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
			/* iteration */
			iterator& operator--() { --index; return *this; }
			/* iteration */
			iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

			/* comparison operators */
			bool operator==(iterator const& other) const { return index == other.index; }
			/* comparison operators */
			bool operator!=(iterator const& other) const { return index != other.index; }
		};

		class const_iterator {
		private:
			/* bound indexed_storage */
			indexed_storage const* storage;
			/* current index */
			size_t index;
			/* constructor */
			const_iterator(indexed_storage const* st, size_t idx) : storage(st), index(idx) {}
			friend class indexed_storage<T>;
		public:
			/* c++ semantics */
			using iterator_category = std::forward_iterator_tag;
			/* c++ semantics */
			using value_type		= const_entry;
			/* c++ semantics */
			using difference_type   = std::ptrdiff_t;
			/* c++ semantics */
			using pointer		   = const_entry*;
			/* c++ semantics */
			using reference		 = const_entry;

			/* constructor */
			const_iterator() : storage(nullptr), index(0) {}

			/* access operator */
			const_entry operator*() const {
				return {storage->lookup[index], storage->objects[index]};
			}

			/* iteration */
			const_iterator& operator++() { ++index; return *this; }
			/* iteration */
			const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
			/* iteration */
			const_iterator& operator--() { --index; return *this; }
			/* iteration */
			const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }

			/* comparison operators */
			bool operator==(const_iterator const& other) const { return index == other.index; }
			/* comparison operators */
			bool operator!=(const_iterator const& other) const { return index != other.index; }
		};

	protected:
		/* vector of held objects */
		std::vector<T> objects;
		/* index to id lookup */
		std::vector<id_t> lookup;
		/* id to index lookup */
		std::vector<size_t> reverse;

		/* next available id */
		id_t next_id = 1;

	private:
		/* init from predefined objects */
		void init_from_objects() {
			size_t n = objects.size();
			lookup.reserve(n);
			reverse.reserve(n + 1);

			reverse.push_back(0);

			for (size_t i = 0; i < n; ++i) {
				id_t new_id = next_id++;
				lookup.push_back(new_id);
				reverse.push_back(i);
			}
		}

	public:
		/* constructor */
		indexed_storage() {
			reverse.push_back(0);
		}

		/* constructor from a T vector */
		explicit indexed_storage(std::vector<T> const& initial_objects)
			: objects(initial_objects) {
			init_from_objects();
		}

		/* constructor from a T vector using move semantics */
		explicit indexed_storage(std::vector<T>&& initial_objects) noexcept
			: objects(std::move(initial_objects)) {
			init_from_objects();
		}

		/* current size */
		id_t size() const { return static_cast<id_t>(objects.size()); }

		/* add element */
		id_t add(T const& object) {
			id_t new_id = next_id++;

			if (new_id >= reverse.size()) {
				reverse.resize(new_id + 1, 0);
			}

			reverse[new_id] = objects.size();
			objects.push_back(object);
			lookup.push_back(new_id);

			return new_id;
		}

		/* add element with move semantics */
		id_t add(T&& object) {
			id_t new_id = next_id++;

			if (new_id >= reverse.size()) {
				reverse.resize(new_id + 1, 0);
			}

			reverse[new_id] = objects.size();
			objects.push_back(std::move(object));
			lookup.push_back(new_id);

			return new_id;
		}

		/* remove an element */
		void remove(id_t id) {
			if (id == nullid || id >= reverse.size() ||
				(reverse[id] == 0 && (objects.empty() || lookup[0] != id))) {
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

		/* access an element */
		T& operator[](id_t id) {
			assert(id != nullid && id < reverse.size() && reverse[id] < objects.size());
			return objects[reverse[id]];
		}

		/* access an element */
		T const& operator[](id_t id) const {
			assert(id != nullid && id < reverse.size() && reverse[id] < objects.size());
			return objects[reverse[id]];
		}

		/* iteration */
		iterator begin() { return iterator(this, 0); }
		/* iteration */
		iterator end() { return iterator(this, objects.size()); }

		/* iteration */
		const_iterator begin() const { return const_iterator(this, 0); }
		/* iteration */
		const_iterator end() const { return const_iterator(this, objects.size()); }

		/* iteration */
		const_iterator cbegin() const { return const_iterator(this, 0); }
		/* iteration */
		const_iterator cend() const { return const_iterator(this, objects.size()); }

		/* remove via an iterator */
		iterator erase(iterator it) {
			if (it != end()) {
				remove((*it).id);
			}
			return it;
		}

		/* remove via a const iterator */
		const_iterator erase(const_iterator it) {
			if (it != cend()) {
				const_cast<indexed_storage*>(this)->remove((*it).id);
			}
			return it;
		}

		/* contains a specific id */
		bool contains(id_t id) const {
			return id != nullid && id < reverse.size() &&
				   (reverse[id] != 0 || (!objects.empty() && lookup[0] == id));
		}
	};
}

