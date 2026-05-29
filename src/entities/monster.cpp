#include "entities/monster.h"
#include "engine/projectile.h"
#include "engine/world.h"
#include "geometry/map-data.h"
#include "geometry/linedef.h"
#include <cmath>

namespace entities {

	monster::monster(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float atk_range, float det_radius, float atk_dmg, float atk_cd, engine::actor* target, geometry::map_data* map, engine::world* world)
		: engine::actor(p, z, tex, is, hp, shield, move_speed, engine::faction::enemy),
		  target_ptr(target),
		  map_ref(map),
		  world_ref(world),
		  attack_cooldown(0.0f),
		  attack_range(atk_range),
		  detection_radius(det_radius),
		  attack_damage(atk_dmg),
		  attack_cd_max(atk_cd) {}

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

	void monster::apply_wall_collision(math::vec2& new_pos) const {
		if (!map_ref) return;
		float r = collision_radius;
		float r2 = r * r;
		for (int iter = 0; iter < 4; ++iter) {
			bool pushed = false;
			for (auto const& e : (*map_ref)("linedefs"_f)) {
				geometry::linedef const& ld = e.value;
				if (ld.is_portal()) continue;
				math::vec2 a = ld("seg"_f)("point0"_f);
				math::vec2 b = ld("seg"_f)("point1"_f);
				math::vec2 cp = closest_on_seg(a, b, new_pos);
				math::vec2 diff = new_pos - cp;
				float d2 = diff.sqr_len();
				if (d2 < r2 && d2 > 1e-8f) {
					float d = std::sqrt(d2);
					new_pos += diff * ((r - d) / d);
					pushed = true;
				}
			}
			if (!pushed) break;
		}
	}

	void monster::move_toward_target(float speed, float dt) {
		math::vec2 new_pos = pos + dir_to_target() * (speed * dt);
		apply_wall_collision(new_pos);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::move_away_from_target(float speed, float dt) {
		math::vec2 new_pos = pos - dir_to_target() * (speed * dt);
		apply_wall_collision(new_pos);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::strafe(float speed, float dt) {
		math::vec2 perp = dir_to_target().perpendicular();
		math::vec2 new_pos = pos + perp * (speed * dt);
		apply_wall_collision(new_pos);
		if (map_ref) map_ref->move_to(this, new_pos);
		else pos = new_pos;
	}

	void monster::melee_attack(float damage) {
		if (target_ptr) target_ptr->take_damage(damage);
	}

	void monster::ranged_attack(float damage, assets::texture_id tex, float speed) {
		if (!target_ptr) return;
		// fallback to direct damage if world/map not available
		if (!world_ref || !map_ref) {
			melee_attack(damage);
			return;
		}

		math::vec2 dir = dir_to_target();
		math::vec2 spawn_pos = pos + dir * 20.0f;

		auto proj = std::make_unique<engine::projectile>(
			spawn_pos, z_pos, tex, 1.5f,
			dir, speed, damage, 5.0f,
			engine::faction::enemy);

		auto* raw = &*proj;
		raw->world_ref = world_ref;
		raw->map_ref = map_ref;

		auto eid = world_ref->register_entity(std::move(proj));
		raw->self_id = eid;

		if ((*map_ref)("root_node_id"_f) != util::indexed_storage<geometry::bsp_node>::nullid) {
			auto sub_id = map_ref->get_subsector_id(spawn_pos);
			(*map_ref)("subsectors"_f)[sub_id].add_sprite(raw);
		}
	}

	void monster::update(float dt) {
		if (is_dead()) return;
		actor::update(dt);
		if (attack_cooldown > 0.0f) attack_cooldown -= dt;
	}

}
