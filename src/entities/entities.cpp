#include "entities/entities.h"

namespace engine::entities {

	void monster::update(float dt)       { actor::update(dt); }
	void monster::take_damage(float dmg) { actor::take_damage(dmg); }

	void player::update(float dt) {
		actor::update(dt);
		if (current_weapon) current_weapon->update(dt);
	}

	void player::take_damage(float dmg) { actor::take_damage(dmg); }

	void player::switch_weapons(int index) {
		if (index < 0 || index >= static_cast<int>(weapons.size())) return;
		current_weapon       = weapons[index];
		current_weapon_index = index;
	}

	void player::shoot() {
		if (current_weapon && current_weapon->can_fire())
			current_weapon->fire({0.0f, 0.0f}, 0.0f);
	}

	void player::reload() {
		if (current_weapon) current_weapon->reload();
	}

	void player::move(math::vec3)    {}
	void player::rotate(float, float) {}
	void player::use_grenade()        {}

}
