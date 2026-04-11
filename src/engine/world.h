#pragma once
#define WORLD_H

#include <vector>
#include <memory>

#include "util/indexed-storage.h"
#include "entity.h"
#include "math/fxpt.h"
#include "math/ray.h"

namespace engine {
	class world {
		util::indexed_storage< std::shared_ptr<entity> > entities;
		util::indexed_storage< std::shared_ptr<rendering::renderable> > renderables;
		std::vector< util::indexed_storage< std::shared_ptr<entity> >::id_t > deleted_entities;
	public:
		world();

		void update(math::fxpt dt);
		util::indexed_storage< std::shared_ptr<rendering::renderable> > const& get_renderable() const;
		util::indexed_storage< std::shared_ptr<entity> >::id_t register_entity(std::shared_ptr<entity> e);
		void delete_entity(util::indexed_storage< std::shared_ptr<entity> >:id_t id);
		util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t raycast(math::ray const r) const;
		std::shared_ptr<rendering::renderable> renderable_from_id(util::indexed_storage< std::shared_ptr<rendering::renderable> >::id_t const id) const;
	};
}

