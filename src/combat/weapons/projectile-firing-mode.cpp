#include "projectile-firing-mode.h"
#include "engine/projectile.h"
#include "engine/world.h"
#include "geometry/map-data.h"
#include <cmath>

namespace combat {
	namespace weapons {

		projectile_firing_mode::projectile_firing_mode(engine::world& w, geometry::map_data& md,
		                                               engine::faction f,
		                                               assets::texture_id tex, float spd,
		                                               float lifetime, float scale)
			: world_ref(w), map_ref(md), team(f),
			  projectile_tex(tex), projectile_speed(spd),
			  projectile_lifetime(lifetime), projectile_scale(scale) {}

		void projectile_firing_mode::spawn_bullet(math::vec2 pos, float angle, float damage) {
			math::vec2 dir{-std::sin(angle), std::cos(angle)};

			math::vec2 spawn_pos = pos + dir * 20.0f;

			auto proj = std::make_unique<engine::projectile>(
				spawn_pos, 0.0f, projectile_tex, projectile_scale,
				dir, projectile_speed, damage, projectile_lifetime,
				team);

			auto* raw = &*proj;
			raw->world_ref = &world_ref;
			raw->map_ref = &map_ref;

			auto eid = world_ref.register_entity(std::move(proj));
			raw->self_id = eid;

			if (map_ref.root_node_id != util::indexed_storage<geometry::bsp_node>::nullid) {
				auto sub_id = map_ref.get_subsector_id(spawn_pos);
				map_ref.subsectors[sub_id].add_sprite(raw);
			}
		}

	}
}
