#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_spawner : public monster, public util::componentized<monster_spawner> {
		[[=util::component_field{}]] int   max_spawns;
		[[=util::component_field{}]] int   current_spawns;
		[[=util::component_field{}]] float spawn_interval;
		float spawn_timer;

		friend class util::componentized<monster_spawner>;
	public:
		using util::componentized<monster_spawner>::operator();
		using util::componentized<monster>::operator();

		monster_spawner() : monster(100.0f, 20.0f, 0.0f, 0.0f, 15.0f),
		    max_spawns(5), current_spawns(0),
		    spawn_interval(5.0f), spawn_timer(0.0f) {}
		void update(float dt) override;
	};

}
