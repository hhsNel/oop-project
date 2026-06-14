#pragma once

#include "engine/actor.h"
#include "assets/ids.h"

namespace engine { class world; }
namespace geometry { class map_data; }

namespace entities {
	class monster : public engine::actor {
	protected:
		/* actor this monster is hunting */
		engine::actor& target;
		/* map used for movement and collision */
		geometry::map_data& map_ref;
		/* world the monster lives in */
		engine::world& world_ref;
		/* time remaining until the next attack */
		float attack_cooldown;
		/* range within which the monster can attack */
		float attack_range;
		/* range within which the monster notices its target */
		float detection_radius;
		/* damage dealt per attack */
		float attack_damage;
		/* cooldown between attacks */
		float attack_cd_max;
		/* radius used for wall collision */
		float collision_radius = 16.0f;

		/* whether the monster currently has a live target */
		bool has_target() const;
		/* distance from the monster to its target */
		float dist_to_target() const;
		/* unit direction from the monster to its target */
		math::vec2 dir_to_target() const;
		/* clamp a candidate position so it does not pass through walls */
		void apply_wall_collision(math::vec2& new_pos) const;
		/* step toward the target this frame */
		void move_toward_target(float speed, float dt);
		/* step away from the target this frame */
		void move_away_from_target(float speed, float dt);
		/* circle around the target this frame */
		void strafe(float speed, float dt);
		/* deal melee damage to the target */
		void melee_attack(float damage);
		/* fire a projectile at the target */
		void ranged_attack(float damage, assets::texture_id tex, float speed);

	public:

		/* constructor */
		monster(math::vec2 const p, float const z,
			assets::texture_id const tex, float const is, float hp,
			float shield, float move_speed, float atk_range,
			float det_radius, float atk_dmg, float atk_cd,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);

		/* entity tick */
		void update(float dt) override;
	};
}
