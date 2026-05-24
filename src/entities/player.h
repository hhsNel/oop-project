#pragma once

#include <vector>

#include "engine/actor.h"
#include "math/vec3.h"
#include "combat/weapons/weapon.h"

int main();

namespace world_object {
	template<typename> class ammo_pickup;
	class weapon_pickup;
}

namespace entities {
	class player : public engine::actor {
		std::vector<combat::weapons::weapon*> weapons;
		float sensitivity;
		combat::weapons::weapon* current_weapon;
		int current_weapon_index;

		friend int ::main();
	public:

		player(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float sens)
			: engine::actor(p, z, tex, is, hp, shield, move_speed, engine::faction::player),
			  sensitivity(sens),
			  current_weapon(nullptr),
			  current_weapon_index(0) {}

		void update(float dt) override;
		void move(math::vec3 direction);
		void rotate(float yaw, float pitch);
		void shoot();
		void reload();
		void switch_weapons(int index);

		template<typename> friend class world_object::ammo_pickup;
		friend class world_object::weapon_pickup;
	};
}
