#pragma once

#include <vector>
#include <memory>

#include "engine/actor.h"
#include "math/vec2.h"
#include "combat/weapons/weapon.h"

namespace world_object {
	class weapon_pickup;
}

namespace entities {
	class player : public engine::actor {
	protected:
		std::vector<std::unique_ptr<combat::weapons::weapon>> weapons;
		float sensitivity;
		int current_weapon_index;
	public:

		player(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float sens)
			: engine::actor(p, z, tex, is, hp, shield, move_speed, engine::faction::player),
			  sensitivity(sens),
			  current_weapon_index(-1) {}

		void update(float dt) override;
		void move(math::vec2 direction);
		void rotate(float yaw);
		void shoot();
		void reload();
		void switch_weapons(int index);

		template<typename WeaponT>
		bool resupply(int amount) {
			for (auto& w : weapons) {
				if (!w) continue;
				if (dynamic_cast<WeaponT*>(&*w)) {
					w->resupply(amount);
					return true;
				}
			}
			return false;
		}

		friend class world_object::weapon_pickup;
	};
}
