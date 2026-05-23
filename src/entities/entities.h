#pragma once
#define ENGINE_ENTITIES_H

#include <vector>
#include <memory>
#include <cmath>

#include "engine/actor.h"
#include "util/indexed-storage.h"
#include "engine/entity.h"
#include "math/vec3.h"
#include "combat/weapons/weapon.h"

namespace world_object {
	class ammo_pickup;
	class weapon_pickup;
}

namespace entities {
	class monster : public engine::actor, public util::componentized<monster> {
		friend class util::componentized<monster>;
	protected:
		[[=util::ref_component_field{}]] std::weak_ptr<engine::actor> target_ptr;
		float attack_cooldown;
		[[=util::component_field{}]] float attack_range;
		[[=util::component_field{}]] float detection_radius;
		[[=util::component_field{}]] float attack_damage;
		[[=util::component_field{}]] float attack_cd_max;

		bool has_target() const;
		float dist_to_target() const;
		math::vec2 dir_to_target() const;
		void move_toward_target(float speed, float dt);
		void move_away_from_target(float speed, float dt);
		void strafe(float speed, float sign, float dt);
		void melee_attack(float damage);
	
	public:

		monster(float hp, float shield, float move_speed, float atk_range, float det_radius, float atk_dmg = 10.0f, float atk_cd = 1.0f)
			: engine::actor(hp, shield, move_speed, engine::faction::enemy),
			  attack_cooldown(0.0f),
			  attack_range(atk_range),
			  detection_radius(det_radius),
			  attack_damage(atk_dmg),
			  attack_cd_max(atk_cd) {};

		void update(float dt) override;
	};

	class player : public engine::actor, public util::componentized<player> {
		[[=util::ref_component_field{}]] std::vector<combat::weapons::weapon*> weapons;
		float sensitivity;
		[[=util::component_field{}]] combat::weapons::weapon* current_weapon;
		[[=util::component_field{}]] int current_weapon_index;

		friend class util::componentized<player>;
	public:

		player(float hp, float shield, float move_speed, float sens)
			: engine::actor(hp, shield, move_speed, engine::faction::player),
			  sensitivity(sens),
			  current_weapon(nullptr),
			  current_weapon_index(0) {}

		void update(float dt) override;
		void move(math::vec3 direction);
		void rotate(float yaw, float pitch);
		void shoot();
		void reload();
		void switch_weapons(int index);

		friend class world_object::ammo_pickup;
		friend class world_object::weapon_pickup;
	};
}
