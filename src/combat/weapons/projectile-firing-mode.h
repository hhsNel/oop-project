#pragma once

#include "firing-mode.h"
#include "engine/actor.h"
#include "assets/ids.h"

namespace engine { class world; }
namespace geometry { class map_data; }

namespace combat {
	namespace weapons {

		class projectile_firing_mode : public firing_mode {
			/* world the projectiles are spawned into */
			engine::world& world_ref;
			/* map passed to each spawned projectile */
			geometry::map_data& map_ref;
			/* faction the spawned projectiles belong to */
			engine::faction team;
			/* texture used for the spawned projectile */
			assets::texture_id projectile_tex;
			/* projectile travel speed */
			float projectile_speed;
			/* seconds before the projectile despawns */
			float projectile_lifetime;
			/* projectile render scale */
			float projectile_scale;

		public:
			/* constructor */
			projectile_firing_mode(engine::world& w, geometry::map_data& md,
			                       engine::faction f,
			                       assets::texture_id tex, float spd,
			                       float lifetime = 5.0f, float scale = 0.5f);

			/* spawn a travelling projectile into the world */
			void spawn_bullet(math::vec2 pos, float angle, float damage) override;
		};

	}
}
