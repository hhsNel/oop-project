#include "entities/entities.h"

namespace engine::entities {

	void monster::update(float dt)   { actor::update(dt); }
	void monster::take_damage(float dmg) { actor::take_damage(dmg); }

	void player::update(float dt)    { actor::update(dt); }
	void player::take_damage(float dmg) { actor::take_damage(dmg); }
	void player::move(math::vec3)    {}
	void player::rotate(float, float) {}
	void player::shoot()             {}
	void player::switch_weapons(int) {}
	void player::use_grenade()       {}

}
