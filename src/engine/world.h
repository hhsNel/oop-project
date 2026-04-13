#pragma once
#define WORLD_H

#include <pair>
#include <vector>
#include <memory>
#include <unordered_map>

#include "util/indexed-storage.h"
#include "entity.h"
#include "rendering/renderable.h"
#include "math/fxpt.h"
#include "math/ray2.h"
#include "math/vec2.h"

namespace engine {
	class world {
		util::indexed_storage< std::shared_ptr<entity> > entities;
		util::indexed_storage< std::shared_ptr<rendering::renderable> > renderables;

		std::vector< util::indexed_storage< std::shared_ptr<entity> >::id_t > deleted_entities;
		std::vector< util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t > deleted_renderables;

		std::unordered_map< util::indexed_storage< std::shared_ptr<entity> >::id_t const, util::indexed_storage< std::shared_ptr<rendering::renderable> > >::id_t const> e_to_r;
		std::unordered_map< util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const, util::indexed_storage< std::shared_ptr<entity> > >::id_t const> r_to_e;
	public:
		world();

		void update(math::fxpt const dt);

		util::indexed_storage< std::shared_ptr<rendering::renderable> > const& get_renderable() const;
		std::pair< util::indexed_storage< std::shared_ptr<entity> >::id_t const, util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const > const register_entity(std::shared_ptr<entity> const e);
		void delete_entity(util::indexed_storage< std::shared_ptr<entity> >:id_t const id);
		util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const register_renderable(std::shared_ptr<rendering::renderable> const r);
		void delete_renderable(util::indexed_storage< std::shared_ptr<rendering::renderable> >:id_t const id);
		std::shared_ptr<entity> const entity_from_id(util::indexed_storage< std::shared_ptr<entity> >::id_t const id) const;
		std::shared_ptr<rendering::renderable> const renderable_from_id(util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const id) const;

		util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const raycast(math::ray2 const r, math::vec2 &hit_point, math::fxpt &distance, math::fxpt &renderable_len) const;

	};
}

