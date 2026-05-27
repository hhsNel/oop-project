#pragma once

#include "firing-mode.h"
#include "engine/actor.h"
#include "assets/ids.h"

namespace engine { class world; }
namespace geometry { class map_data; }

namespace combat {
	namespace weapons {

		class projectile_firing_mode : public firing_mode {
			engine::world& world_ref;
			geometry::map_data& map_ref;
			engine::faction team;
			assets::texture_id projectile_tex;
			float projectile_speed;
			float projectile_lifetime;
			float projectile_scale;

		public:
			projectile_firing_mode(engine::world& w, geometry::map_data& md,
			                       engine::faction f,
			                       assets::texture_id tex, float spd,
			                       float lifetime = 5.0f, float scale = 0.5f);

			void spawn_bullet(math::vec2 pos, float angle, float damage) override;
		};

	}
}
