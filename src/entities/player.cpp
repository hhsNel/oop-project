#include "entities/player.h"
#include <cmath>
#include <numbers>

namespace entities {

	void player::update(float dt) {
		actor::update(dt);
		for (auto& w : weapons) if (w) w->tick(dt);
	}

	void player::switch_weapons(int index) {
		if (index < 0 || index >= static_cast<int>(weapons.size())) return;
		current_weapon_index = index;
	}

	void player::shoot(std::span<engine::actor*> targets) {
		if (current_weapon_index < 0) return;
		auto& w = weapons[current_weapon_index];
		if (w && w->can_fire())
			w->fire(pos, angle, targets);
	}

	void player::reload() {
		if (current_weapon_index < 0) return;
		auto& w = weapons[current_weapon_index];
		if (w) w->reload();
	}

	void player::move(math::vec2 direction) {
		float ca = std::cos(angle);
		float sa = std::sin(angle);
		math::vec2 move_dir{
			direction("y"_f) * ca + direction("x"_f) * sa,
			direction("y"_f) * sa - direction("x"_f) * ca
		};
		pos += move_dir * movement_speed;
	}

	void player::rotate(float yaw) {
		angle += yaw * sensitivity;
		// keep angle in [0, 2π)
		angle = std::fmod(angle, 2.0f * std::numbers::pi_v<float>);
		if (angle < 0.0f) angle += 2.0f * std::numbers::pi_v<float>;
	}

}
