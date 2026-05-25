#pragma once
#include <vector>
#include <memory>

#include "util/indexed-storage.h"
#include "entity.h"

namespace engine {
	class world {
	private:
		util::indexed_storage<std::unique_ptr<entity>> entities;
		std::vector<util::indexed_storage<std::unique_ptr<entity>>::id_t> deleted_entities;

	public:

		world(const world&) = delete;
		world& operator=(const world&) = delete;

		void update(float const dt);

		entity& operator[](util::indexed_storage<std::unique_ptr<entity>>::id_t const id);
		entity const& operator[](util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const;

		util::indexed_storage<std::unique_ptr<entity>> const& get_entities() const;

		util::indexed_storage<std::unique_ptr<entity>>::id_t register_entity(std::unique_ptr<entity> e);

		void delete_entity(util::indexed_storage<std::unique_ptr<entity>>::id_t const id);

		entity* entity_from_id(util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const;
	};
}

