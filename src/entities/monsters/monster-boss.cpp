#include "monster-boss.h"
#include "monster-basic.h"
#include "combat/burning.h"
#include "combat/slowed.h"
#include "engine/projectile.h"
#include "engine/world.h"
#include <cmath>

namespace entities {

monster_boss::monster_boss(math::vec2 const p, float const z)
	: monster(p, z, 3, 1.0f, 1000.0f, 300.0f, 70.0f, 350.0f, 500.0f, 20.0f, 1.0f),
	  max_hp_val(1000.0f) {}

bool monster_boss::channeling() const { return is_channeling; }
float monster_boss::flash_phase() const { return flash_timer; }

float monster_boss::hp_ratio() const {
	float cur = health("current_hp"_f);
	return cur / max_hp_val;
}

void monster_boss::update_phase() {
	float ratio = hp_ratio();
	if (ratio <= 0.25f && current_phase < 3)
		current_phase = 3;
	else if (ratio <= 0.5f && current_phase < 2)
		current_phase = 2;
}

bool monster_boss::minions_alive() const {
	if (!world_ref) return false;
	for (auto id : minion_ids) {
		auto* e = world_ref->entity_from_id(id);
		if (e) {
			auto* a = dynamic_cast<engine::actor*>(e);
			if (a && !a->is_dead()) return true;
		}
	}
	return false;
}

void monster_boss::ranged_attack_slow() {
	if (!target_ptr || !world_ref || !boss_map_ref) return;

	math::vec2 dir = dir_to_target();
	float base_angle = std::atan2(dir("y"_f), dir("x"_f));

	float start = base_angle - projectile_spread * (projectile_count - 1) / 2.0f;
	for (int i = 0; i < projectile_count; ++i) {
		float a = start + projectile_spread * static_cast<float>(i);
		math::vec2 shot_dir{std::cos(a), std::sin(a)};
		math::vec2 spawn_pos = pos + shot_dir * 20.0f;

		auto proj = std::make_unique<engine::projectile>(
			spawn_pos, z_pos, 30, 0.5f,
			shot_dir, 250.0f, attack_damage * 0.5f, 5.0f,
			engine::faction::enemy);

		auto* raw = &*proj;
		raw->world_ref = world_ref;
		raw->map_ref = boss_map_ref;
		raw->on_hit_effect = []() { return std::make_unique<combat::slowed>(3.0f, 30); };

		auto eid = world_ref->register_entity(std::move(proj));
		raw->self_id = eid;

		if (boss_map_ref->root_node_id != util::indexed_storage<geometry::bsp_node>::nullid) {
			auto sub_id = boss_map_ref->get_subsector_id(spawn_pos);
			boss_map_ref->subsectors[sub_id].add_sprite(raw);
		}
	}
}

void monster_boss::melee_attack_burn() {
	if (!target_ptr) return;
	melee_attack(attack_damage);
	target_ptr->add_effect(std::make_unique<combat::burning>(3.0f, 5));
}

void monster_boss::start_charge() {
	if (!has_target()) return;
	is_charging = true;
	charge_timer = charge_duration;
	charge_dir = dir_to_target();
}

void monster_boss::update_charge(float dt) {
	math::vec2 new_pos = pos + charge_dir * (movement_speed * charge_speed_mult * dt);
	apply_wall_collision(new_pos);
	if (boss_map_ref) boss_map_ref->move_to(this, new_pos);
	else pos = new_pos;
	charge_timer -= dt;

	float dist = dist_to_target();
	if (dist <= attack_range) {
		melee_attack(attack_damage * 2.0f);
		is_charging = false;
		post_charge_stun = stun_duration;
		return;
	}

	if (charge_timer <= 0.0f) {
		is_charging = false;
		post_charge_stun = stun_duration;
	}
}

void monster_boss::start_channel() {
	if (!world_ref) return;
	is_channeling = true;
	channel_timer = 0.0f;
	flash_timer = 0.0f;
	minion_ids.clear();

	for (int i = 0; i < 4; ++i) {
		float angle_offset = static_cast<float>(i) * (std::numbers::pi_v<float> / 2.0f);
		math::vec2 spawn_pos = pos + math::vec2{
			std::cos(angle_offset) * 40.0f,
			std::sin(angle_offset) * 40.0f
		};

		auto minion = std::make_unique<monster_basic>(spawn_pos, z_pos, target_ptr);

		auto* raw = &*minion;
		auto id = world_ref->register_entity(std::move(minion));
		minion_ids.push_back(id);

		if (boss_map_ref) {
			auto sub_id = boss_map_ref->get_subsector_id(spawn_pos);
			boss_map_ref->subsectors[sub_id].add_sprite(raw);
		}
	}
}

void monster_boss::phase1_ai(float dt, float dist) {
	stance_timer -= dt;
	if (stance_timer <= 0.0f) {
		wants_melee = !wants_melee;
		stance_timer = 10.0f;
	}

	if (wants_melee) {
		if (dist > 60.0f)
			move_toward_target(movement_speed * 1.3f, dt);

		if (dist <= 60.0f && attack_cooldown <= 0.0f) {
			melee_attack_burn();
			attack_cooldown = attack_cd_max;
		}
	} else {
		float preferred = attack_range * 0.7f;
		if (dist > preferred + 1.5f)
			move_toward_target(movement_speed, dt);
		else if (dist < preferred - 1.5f)
			move_away_from_target(movement_speed, dt);

		ranged_cooldown -= dt;
		if (dist <= attack_range && ranged_cooldown <= 0.0f) {
			ranged_attack_slow();
			ranged_cooldown = ranged_cd_max;
		}
	}
}

void monster_boss::phase2_ai(float dt, float dist) {
	// charge
	if (!is_charging && charge_cooldown <= 0.0f && dist > attack_range * 0.5f) {
		start_charge();
		charge_cooldown = charge_cd_max;
		return;
	}

	// between charges, phase 1 behavior
	phase1_ai(dt, dist);
}

void monster_boss::phase3_ai(float dt, float dist) {
	// entering phase 3, start channel
	if (!phase3_debuffed && !is_channeling) {
		start_channel();
		return;
	}

	// channeling
	if (is_channeling) {
		flash_timer += dt * 4.0f;
		if (!minions_alive()) {
			is_channeling = false;
			phase3_debuffed = true;
			movement_speed *= 0.6f;
		}
		return;
	}

	// post-channel
	if (!is_charging && charge_cooldown <= 0.0f && dist > attack_range) {
		start_charge();
		charge_cooldown = charge_cd_max;
		return;
	}

	if (dist > attack_range)
		move_toward_target(movement_speed, dt);

	if (dist <= attack_range && attack_cooldown <= 0.0f) {
		melee_attack_burn();
		attack_cooldown = attack_cd_max;
	}
}

void monster_boss::update(float dt) {
	monster::update(dt);
	if (is_dead()) return;
	if (!has_target()) return;

	update_phase();

	float dist = dist_to_target();
	if (dist > detection_radius) return;

	// charge in progress
	if (is_charging) {
		update_charge(dt);
		return;
	}

	// post-charge stun
	if (post_charge_stun > 0.0f) {
		post_charge_stun -= dt;
		return;
	}

	// channeling
	if (is_channeling) {
		phase3_ai(dt, dist);
		return;
	}

	charge_cooldown -= dt;

	switch (current_phase) {
		case 1: phase1_ai(dt, dist); break;
		case 2: phase2_ai(dt, dist); break;
		case 3: phase3_ai(dt, dist); break;
	}
}

}
