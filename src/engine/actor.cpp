#include "actor.h"

namespace engine {

	void actor::take_damage(float const dmg) { health.apply_damage(dmg); }
	void actor::take_true_damage(float const dmg) { health.apply_true_damage(dmg); }
	void actor::heal(float const amount) { health.apply_heal(amount); }
	void actor::add_shield(float const amount) { health.apply_shield(amount); }

	void actor::add_effect(std::unique_ptr<combat::status_effect> effect) {
		health.add_effect(std::move(effect), *this);
	}

	void actor::update(float dt) {
		health.process_effects(dt, *this);
	}

}
