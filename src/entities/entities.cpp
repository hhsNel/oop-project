#include "entities/entities.h"
#include <cmath>
#include <algorithm>
#include <numbers>

namespace entities {

	bool monster::has_target() const {
		auto t = target_ptr.lock();
		return t && !t->is_dead();
	}

	float monster::dist_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return 0.0f;
		math::vec2 d = (*t)("pos"_f) - pos;
		return std::hypot(d("x"_f), d("y"_f));

	}

	math::vec2 monster::dir_to_target() const {
		auto t = target_ptr.lock();
		if (!t) return {0.0f, 0.0f};
		math::vec2 d = (*t)("pos"_f) - pos;
		float len = std::hypot(d("x"_f), d("y"_f));

		if (len < 0.0001f) return {0.0f, 0.0f};
		return d / len;
	}

	void monster::move_toward_target(float speed, float dt) {
		pos += dir_to_target() * (speed * dt);
	}

	void monster::move_away_from_target(float speed, float dt) {
		pos -= dir_to_target() * (speed * dt);
	}

	void monster::strafe(float speed, float sign, float dt) {
		math::vec2 d = dir_to_target();
		math::vec2 perp = d.perpendicular() * sign;
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

	void player::update(float dt) {
		actor::update(dt);
		for (auto* w : weapons) if (w) w->update(dt);
	}

	void player::switch_weapons(int index) {
		if (index < 0 || index >= static_cast<int>(weapons.size())) return;
		current_weapon       = weapons[index];
		current_weapon_index = index;
	}

	void player::shoot() {
		if (current_weapon && current_weapon->can_fire())
			current_weapon->fire(pos, angle);
	}

	void player::reload() {
		if (current_weapon) current_weapon->reload();
	}

	void player::move(math::vec3 direction) {
		float ca = std::cos(angle);
		float sa = std::sin(angle);
		pos += math::vec2{
			(direction("y"_f) * ca + direction("x"_f) * sa) * movement_speed,
			(direction("y"_f) * sa - direction("x"_f) * ca) * movement_speed
		};
	}

	void player::rotate(float yaw, float /*pitch*/) {
		angle += yaw * sensitivity;
		// keep angle in [0, 2π)
		angle = std::fmod(angle, 2.0f * std::numbers::pi_v<float>);
		if (angle < 0.0f) angle += 2.0f * std::numbers::pi_v<float>;
	}

}
