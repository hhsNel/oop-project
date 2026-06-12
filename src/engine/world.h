#pragma once
#include <vector>
#include <memory>

#include "util/indexed-storage.h"
#include "entity.h"

namespace engine {
	class world {
	private:
		/* all entities */
		util::indexed_storage<std::unique_ptr<entity>> entities;
		/* entities to be removed at the end of a tick */
		std::vector<util::indexed_storage<std::unique_ptr<entity>>::id_t>
			deleted_entities;

	public:
		
		/* constructor */
		world();

		world(const world&) = delete;
		world& operator=(const world&) = delete;

		/* tick all entities */
		void update(float const dt);

		/* get an entity */
		entity& operator[](
			util::indexed_storage<std::unique_ptr<entity>>::id_t const id);
		/* get an entity */
		entity const& operator[](
			util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const;
		/* all entities */
		util::indexed_storage<std::unique_ptr<entity>> const& get_entities() const;

		/* add an entity */
		util::indexed_storage<std::unique_ptr<entity>>::id_t
			register_entity(std::unique_ptr<entity> e);

		/* delete an entity */
		void delete_entity(
			util::indexed_storage<std::unique_ptr<entity>>::id_t const id);

		/* entity ptr (can be NULL) */
		entity* entity_from_id(
			util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const;
	};
}

