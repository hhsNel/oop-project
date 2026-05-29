#pragma once

#include "engine/actor.h"
#include "assets/ids.h"

namespace engine { class world; }
namespace geometry { class map_data; }

namespace entities {
	class monster : public engine::actor {
	protected:
		engine::actor* target_ptr = nullptr;
		geometry::map_data* map_ref = nullptr;
		engine::world* world_ref = nullptr;
		float attack_cooldown;
		float attack_range;
		float detection_radius;
		float attack_damage;
		float attack_cd_max;
		float collision_radius = 16.0f;

		bool has_target() const;
		float dist_to_target() const;
		math::vec2 dir_to_target() const;
		void apply_wall_collision(math::vec2& new_pos) const;
		void move_toward_target(float speed, float dt);
		void move_away_from_target(float speed, float dt);
		void strafe(float speed, float dt);
		void melee_attack(float damage);
		void ranged_attack(float damage, assets::texture_id tex, float speed);

	public:

		monster(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float atk_range, float det_radius, float atk_dmg, float atk_cd, engine::actor* target = nullptr, geometry::map_data* map = nullptr, engine::world* world = nullptr);

		void update(float dt) override;
	};
}
