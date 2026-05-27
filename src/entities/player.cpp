#include "entities/player.h"
#include "geometry/map-data.h"
#include "engine/world.h"
#include <cmath>
#include <numbers>

namespace entities {

	void player::update(float dt) {
		actor::update(dt);
		for (auto& w : weapons) if (w) w->tick(dt);
	}

	void player::switch_weapons(int index) {
		if (index < 0 || index >= static_cast<int>(weapons.size())) return;
		if (!weapons[index]) return;
		current_weapon_index = index;
	}

	void player::shoot() {
		if (current_weapon_index < 0) return;
		auto& w = weapons[current_weapon_index];
		if (w && w->can_fire())
			w->fire(pos, angle);
	}

	void player::reload() {
		if (current_weapon_index < 0) return;
		auto& w = weapons[current_weapon_index];
		if (w) w->reload();
	}

	// Closest point on segment AB to point P
	static math::vec2 closest_on_seg(math::vec2 a, math::vec2 b, math::vec2 p) {
		math::vec2 ab = b - a;
		float ab_sq = ab.sqr_len();
		if (ab_sq < 1e-8f) return a;
		float t = math::vec2::dot_product(p - a, ab) / ab_sq;
		if (t < 0.0f) t = 0.0f;
		if (t > 1.0f) t = 1.0f;
		return a + ab * t;
	}

	void player::move(math::vec2 direction) {
		float ca = std::cos(angle);
		float sa = std::sin(angle);
		math::vec2 move_dir{
			direction("y"_f) * ca + direction("x"_f) * sa,
			direction("y"_f) * sa - direction("x"_f) * ca
		};
		math::vec2 new_pos = pos + move_dir * movement_speed;

		if (map_ref) {
			float r = collision_radius;
			float r2 = r * r;

			// Wall collision — slide along walls
			for (int iter = 0; iter < 4; ++iter) {
				bool pushed = false;
				for (auto const& e : map_ref->linedefs) {
					geometry::linedef const& ld = e.value;
					if (ld.is_portal()) continue;

					math::vec2 a = ld("seg"_f)("point0"_f);
					math::vec2 b = ld("seg"_f)("point1"_f);
					math::vec2 cp = closest_on_seg(a, b, new_pos);
					math::vec2 diff = new_pos - cp;
					float d2 = diff.sqr_len();

					if (d2 < r2 && d2 > 1e-8f) {
						float d = std::sqrt(d2);
						math::vec2 push = diff * ((r - d) / d);
						new_pos += push;
						pushed = true;
					}
				}
				if (!pushed) break;
			}

			// Monster collision
			if (world_ref) {
				for (auto [id, entity_ptr] : world_ref->get_entities()) {
					if (!entity_ptr) continue;
					auto* a = dynamic_cast<engine::actor*>(&*entity_ptr);
					if (!a || a == this || a->is_dead()) continue;

					math::vec2 diff = new_pos - (*a)("pos"_f);
					float d2 = diff.sqr_len();
					float min_dist = r + 16.0f;  // monster radius ~16

					if (d2 < min_dist * min_dist && d2 > 1e-8f) {
						float d = std::sqrt(d2);
						math::vec2 push = diff * ((min_dist - d) / d);
						new_pos += push;
					}
				}
			}
		}

		pos = new_pos;
	}

	void player::rotate(float yaw) {
		angle += yaw * sensitivity;
		// keep angle in [0, 2π)
		angle = std::fmod(angle, 2.0f * std::numbers::pi_v<float>);
		if (angle < 0.0f) angle += 2.0f * std::numbers::pi_v<float>;
	}

}
