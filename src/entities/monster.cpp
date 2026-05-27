#include "entities/monster.h"
<<<<<<< HEAD
#include "engine/projectile.h"
#include "engine/world.h"
=======
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
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
			spawn_pos, z_pos, tex, 0.5f,
			dir, speed, damage, 5.0f,
			engine::faction::enemy);

		auto* raw = &*proj;
		raw->world_ref = world_ref;
		raw->map_ref = map_ref;

		auto eid = world_ref->register_entity(std::move(proj));
		raw->self_id = eid;

		if (map_ref->root_node_id != util::indexed_storage<geometry::bsp_node>::nullid) {
			auto sub_id = map_ref->get_subsector_id(spawn_pos);
			map_ref->subsectors[sub_id].add_sprite(raw);
		}
	}

	void monster::update(float dt) {
		if (is_dead()) return;
		actor::update(dt);
		if (attack_cooldown > 0.0f) attack_cooldown -= dt;
	}

}
