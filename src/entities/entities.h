#pragma once
#define ENGINE_ENTITIES_H

#include <vector>
#include <memory>

#include "engine/actor.h"
#include "engine/projectile.h"
#include "combat/combat.h"
#include "util/indexed-storage.h"
#include "engine/entity.h"

namespace engine {
	namespace entities {
		class monster : public actor {
		public:
			float attack_range;
			float detection_radius;
			util::indexed_storage< std::shared_ptr<entity> >::id_t target;

			monster(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
				: actor(hp, shield, movement_speed, faction::enemy),
				  attack_range(attack_range),
				  detection_radius(detection_radius) {}

			void update(float dt) override;
			void take_damage(float dmg) override;
		};

		class plasma_projectile : public projectile {
		public:
			float speed;
			int damage;

		};

		class grenade_projectile : public projectile {
		public:
			float fuse_time;
			float explosion_radius;
		};

		class slug_projectile : public projectile {};

		class player : public actor {
		public:
			float sensitivity;

			std::vector<combat::weapon*> weapons;
			combat::weapon* current_weapon;
			int current_weapon_index;

			player(float hp, float shield, float movement_speed, float sensitivity)
				: actor(hp,shield,movement_speed, faction::player),
				  sensitivity(sensitivity),
				  current_weapon(nullptr),
				  current_weapon_index(0) {}

			void update(float dt) override;
			void move(math::vec3 direction);
			void rotate(float yaw, float pitch);
			void shoot();
			void switch_weapons(int index);
			void use_grenade();

			void take_damage(float dmg) override;
		};
	}
}
