#pragma once
#define ENGINE_ENTITIES_H

#include <vector>
#include <memory>
#include <cmath>

#include "engine/actor.h"
#include "combat/combat.h"
#include "util/indexed-storage.h"
#include "engine/entity.h"
#include "math/vec3.h"

namespace engine {
	namespace entities {
		class monster : public actor {
		protected:
			std::weak_ptr<actor> target_ptr;
			float attack_cooldown;

			bool has_target() const;
			float dist_to_target() const;
			math::vec2 dir_to_target() const;
			void move_toward_target(float speed, float dt);
			void move_away_from_target(float speed, float dt);
			void strafe(float speed, float sign, float dt);
			void melee_attack(float damage);

		public:
			float attack_range;
			float detection_radius;
			float attack_damage;
			float attack_cd_max;

			monster(float hp, float shield, float move_speed, float atk_range, float det_radius)
				: actor(hp, shield, move_speed, faction::enemy),
				  attack_cooldown(0.0f),
				  attack_range(atk_range),
				  detection_radius(det_radius),
				  attack_damage(10.0f),
				  attack_cd_max(1.0f) {}

			void set_target(std::shared_ptr<actor> t);

			void update(float dt) override;
			void take_damage(float dmg) override;
		};

		class player : public actor {
		public:
			float sensitivity;

			std::vector<combat::weapon*> weapons;
			combat::weapon* current_weapon;
			int current_weapon_index;

			player(float hp, float shield, float move_speed, float sens)
				: actor(hp, shield, move_speed, faction::player),
				  sensitivity(sens),
				  current_weapon(nullptr),
				  current_weapon_index(0) {}

			void update(float dt) override;
			void move(math::vec3 direction);
			void rotate(float yaw, float pitch);
			void shoot();
			void reload();
			void switch_weapons(int index);
			void use_grenade();

			void take_damage(float dmg) override;
		};
	}
}
