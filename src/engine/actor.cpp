#include "actor.h"

namespace engine {

	actor::actor(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float const hp, float const shield, float const move_speed, faction const this_team)
		: renderable_entity(p, z, tex, is), health(hp, hp, shield, shield), movement_speed(move_speed), team(this_team) {}

	bool actor::is_dead() const { return health.is_dead(); }

	void actor::take_damage(float const dmg) {
		health.apply_damage(dmg);
		hit_flash = 0.15f;
	}
	void actor::take_true_damage(float const dmg) { health.apply_true_damage(dmg); }
	void actor::heal(float const amount) { health.apply_heal(amount); }
	void actor::add_shield(float const amount) { health.apply_shield(amount); }

	void actor::add_effect(std::unique_ptr<combat::status_effect> effect) {
		health.add_effect(std::move(effect), *this);
	}

	void actor::update(float dt) {
		health.process_effects(dt, *this);
		if (hit_flash > 0.0f) hit_flash -= dt;
	}

}
