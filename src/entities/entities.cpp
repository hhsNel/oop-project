#include "entities/entities.h"
#include <cmath>
#include <algorithm>

namespace engine::entities {

	void monster::set_target(std::shared_ptr<actor> t) { target_ptr = t; }

	bool monster::has_target() const {
		auto t = target_ptr.lock();
		return t && !t->is_dead();
	}

	float monster::dist_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return 0.0f;
		math::vec2 d = t->position - position;
		return std::sqrt(d.x * d.x + d.y * d.y);
	}

	math::vec2 monster::dir_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return {0.0f, 0.0f};
		math::vec2 d = t->position - position;
		float len = std::sqrt(d.x * d.x + d.y * d.y);
		if (len < 0.0001f) return {0.0f, 0.0f};
		return d / len;
	}

	void monster::move_toward_target(float speed, float dt) {
		position += dir_to_target() * (speed * dt);
	}

	void monster::move_away_from_target(float speed, float dt) {
		position -= dir_to_target() * (speed * dt);
	}

	void monster::strafe(float speed, float sign, float dt) {
		math::vec2 d = dir_to_target();
		math::vec2 perp = d.perpendicular() * sign;
		position += perp * (speed * dt);
	}

	void monster::melee_attack(float damage) {
		auto t = target_ptr.lock();
		if (t) t->take_damage(damage);
	}

	void monster::update(float dt) {
		actor::update(dt);
		if (attack_cooldown > 0.0f) attack_cooldown -= dt;
	}

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
