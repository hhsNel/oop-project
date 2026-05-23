#include "systems.h"
#include <algorithm>

namespace systems {

	health_system::health_system() : current_hp(0), max_hp(0), armor(0), max_armor(0) {}

	health_system::health_system(float hp, float max, float arm, float max_arm)
		: current_hp(hp), max_hp(max), armor(arm), max_armor(max_arm) {}

	void health_system::apply_damage(float amount) {
		if (armor > 0.0f) {
			float absorbed = std::min(armor, amount);
			armor -= absorbed;
			amount -= absorbed;
		}
		current_hp = std::max(0.0f, current_hp - amount);
	}

	void health_system::apply_heal(float amount) {
		current_hp = std::min(max_hp, current_hp + amount);
	}

	void health_system::apply_shield(float amount) {
		armor = std::min(max_armor, armor + amount);
	}

	void health_system::apply_true_damage(float amount) {
		current_hp = std::max(0.0f, current_hp - amount);
	}

	void health_system::add_effect(std::unique_ptr<combat::status_effect> effect, engine::actor& owner) {
		effect->on_apply(owner);
		active_effects.push_back(std::move(effect));
	}

	void health_system::process_effects(float dt, engine::actor& owner) {
		for (auto& effect : active_effects) {
			bool should_tick = !effect->update(dt);
			if (should_tick) {
				effect->affect(owner);
			}
			if (effect->is_expired()) {
				effect->on_expire(owner);
			}
		}

		std::erase_if(active_effects,
			[](const auto& e) { return e->is_expired(); });
	}

}
