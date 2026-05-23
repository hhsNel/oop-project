#include "world.h"

namespace engine {

	world::world() {
		/* TODO */
	}

	world::~world() {
		/* TODO */
	}

	void world::update(float const dt) {
		for (auto [id, entity_ptr] : entities) {
			if (entity_ptr) {
				entity_ptr->update(dt);
			}
		}

		for (auto const id : deleted_entities) {
			entities.remove(id);
		}
		deleted_entities.clear();
	}

	entity& world::operator[](util::indexed_storage<std::unique_ptr<entity>>::id_t const id) {
		return *entities[id];
	}

	entity const& world::operator[](util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const {
		return *entities[id];
	}

	util::indexed_storage<std::unique_ptr<entity>> const& world::get_entities() const {
		return entities;
	}

	util::indexed_storage<std::unique_ptr<entity>>::id_t world::register_entity(std::unique_ptr<entity> e) {
		return entities.add(std::move(e));
	}

	void world::delete_entity(util::indexed_storage<std::unique_ptr<entity>>::id_t const id) {
		deleted_entities.push_back(id);
	}

	entity* world::entity_from_id(util::indexed_storage<std::unique_ptr<entity>>::id_t const id) const {
		if (entities.contains(id)) {
			return entities[id].get();
		}
		return nullptr;
	}

}
