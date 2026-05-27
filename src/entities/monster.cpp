#include "entities/monster.h"
#include "geometry/map-data.h"
#include <cmath>

namespace entities {

	bool monster::has_target() const {
		return target_ptr && !target_ptr->is_dead();
	}

	float monster::dist_to_target() const {
		if (!target_ptr) return 0.0f;
		math::vec2 d = (*target_ptr)("pos"_f) - pos;
		return d.len();
	}

	math::vec2 monster::dir_to_target() const {
		if (!target_ptr) return {0.0f, 0.0f};
		math::vec2 d = (*target_ptr)("pos"_f) - pos;
		return d.normalized();
	}

	void monster::move_toward_target(float speed, float dt) {
		math::vec2 new_pos = pos + dir_to_target() * (speed * dt);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::move_away_from_target(float speed, float dt) {
		math::vec2 new_pos = pos - dir_to_target() * (speed * dt);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::strafe(float speed, float dt) {
		math::vec2 perp = dir_to_target().perpendicular();
		math::vec2 new_pos = pos + perp * (speed * dt);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::melee_attack(float damage) {
		if (target_ptr) target_ptr->take_damage(damage);
	}

	void monster::update(float dt) {
		if (is_dead()) return;
		actor::update(dt);
		if (attack_cooldown > 0.0f) attack_cooldown -= dt;
	}

}
