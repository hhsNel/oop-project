#include "entities/monster.h"
#include <cmath>

namespace entities {

	bool monster::has_target() const {
		auto t = target_ptr.lock();
		return t && !t->is_dead();
	}

	float monster::dist_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return 0.0f;
		math::vec2 d = (*t)("pos"_f) - pos;
		return d.len();
	}

	math::vec2 monster::dir_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return {0.0f, 0.0f};
		math::vec2 d = (*t)("pos"_f) - pos;
		return d.normalized();
	}

	void monster::move_toward_target(float speed, float dt) {
		pos += dir_to_target() * (speed * dt);
	}

	void monster::move_away_from_target(float speed, float dt) {
		pos -= dir_to_target() * (speed * dt);
	}

	void monster::strafe(float speed, float dt) {
		math::vec2 perp = dir_to_target().perpendicular();
		pos += perp * (speed * dt);
	}

	void monster::melee_attack(float damage) {
		auto t = target_ptr.lock();
		if (t) t->take_damage(damage);
	}

	void monster::update(float dt) {
		actor::update(dt);
		if (attack_cooldown > 0.0f) attack_cooldown -= dt;
	}

}
