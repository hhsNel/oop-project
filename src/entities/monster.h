#pragma once

#include "engine/actor.h"

int main();

namespace entities {
	class monster : public engine::actor {
	protected:
		engine::actor* target_ptr = nullptr;
		float attack_cooldown;
		float attack_range;
		float detection_radius;
		float attack_damage;
		float attack_cd_max;

		friend int ::main();

		bool has_target() const;
		float dist_to_target() const;
		math::vec2 dir_to_target() const;
		void move_toward_target(float speed, float dt);
		void move_away_from_target(float speed, float dt);
		void strafe(float speed, float dt);
		void melee_attack(float damage);

	public:

		monster(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float atk_range, float det_radius, float atk_dmg, float atk_cd)
			: engine::actor(p, z, tex, is, hp, shield, move_speed, engine::faction::enemy),
			  attack_cooldown(0.0f),
			  attack_range(atk_range),
			  detection_radius(det_radius),
			  attack_damage(atk_dmg),
			  attack_cd_max(atk_cd) {};

		void update(float dt) override;
	};
}
