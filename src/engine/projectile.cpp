#include "projectile.h"
#include "world.h"
#include "geometry/map-data.h"
#include "geometry/linedef.h"
#include "geometry/bsp-node.h"
#include "math/ray2.h"
#include <cmath>

namespace engine {

projectile::projectile(math::vec2 p, float z, assets::texture_id tex, float scale,
                       math::vec2 dir, float spd, float dmg, float life,
                       faction f, float radius)
	: renderable_entity(p, z, tex, scale),
	  direction(dir), speed(spd), damage(dmg), lifetime(life),
	  hit_radius(radius), team(f) {}

void projectile::update(float dt) {
	if (lifetime <= 0.0f) return;

	bool has_bsp = map_ref &&
		map_ref->root_node_id != util::indexed_storage<geometry::bsp_node>::nullid;

	auto remove_and_delete = [&]() {
		if (has_bsp) {
			auto sub_id = map_ref->get_subsector_id(pos);
			map_ref->subsectors[sub_id].remove_sprite(this);
		}
		lifetime = 0.0f;
		if (world_ref && self_id != 0)
			world_ref->delete_entity(self_id);
	};

	lifetime -= dt;
	if (lifetime <= 0.0f) {
		remove_and_delete();
		return;
	}

	math::vec2 new_pos = pos + direction * (speed * dt);

	// wall collision
	if (map_ref) {
		math::ray2 ray{pos, direction};
		float move_dist = speed * dt;

		for (auto const& e : map_ref->linedefs) {
			geometry::linedef const& ld = e.value;
			if (ld.is_portal()) continue;
			math::vec2 hit;
			float dist = 0.0f, seg_len = 0.0f;
			if (ray.intersects(ld("seg"_f), hit, dist, seg_len) && dist > 0.0f && dist <= move_dist) {
				remove_and_delete();
				return;
			}
		}
	}

	// actor collision
	if (world_ref) {
		for (auto [id, entity_ptr] : world_ref->get_entities()) {
			if (!entity_ptr) continue;
			auto* a = dynamic_cast<actor*>(&*entity_ptr);
			if (!a || a->is_dead()) continue;
			if (a->team == team) continue;

			math::vec2 diff = (*a)("pos"_f) - new_pos;
			if (diff.sqr_len() < hit_radius * hit_radius) {
				a->take_damage(damage);
				if (on_hit_effect) a->add_effect(on_hit_effect());
				remove_and_delete();
				return;
			}
		}
	}

	// move
	if (map_ref)
		map_ref->move_to(this, new_pos);
	else
		pos = new_pos;
}

}
