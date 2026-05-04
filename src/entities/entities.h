#pragma once
#define ENGINE_ENTITIES_H

#include <vector>
#include <memory>

#include "engine/actor.h"
#include "combat/combat.h"
#include "util/indexed-storage.h"
#include "engine/entity.h"
#include "math/vec3.h"

namespace engine {
	namespace entities {
		class monster : public actor {
		public:
			float attack_range;
			float detection_radius;
			util::indexed_storage< std::shared_ptr<entity> >::id_t target;

			monster(float hp, float shield, float move_speed, float atk_range, float det_radius)
				: actor(hp, shield, move_speed, faction::enemy),
				  attack_range(atk_range),
				  detection_radius(det_radius) {}

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
			void switch_weapons(int index);
			void use_grenade();

			void take_damage(float dmg) override;
		};
	}
}
