#include "systems.h"
#include <algorithm>

namespace systems {

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

		active_effects.erase(
			std::remove_if(active_effects.begin(), active_effects.end(),
				[](const auto& e) { return e->is_expired(); }),
			active_effects.end()
		);
	}

}
