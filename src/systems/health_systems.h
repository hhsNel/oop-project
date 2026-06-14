#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "combat/status-effect.h"
#include "util/indexed-storage.h"
#include "util/componentized.h"

namespace engine {class actor;}

namespace systems {
	class health_system : public util::componentized<health_system> {
		[[=util::component_field{}]] float current_hp;
		[[=util::component_field{}]] float max_hp;
		[[=util::component_field{}]] float armor;
		[[=util::component_field{}]] float max_armor;

		[[=util::component_field{}]] std::vector<std::unique_ptr<combat::status_effect>> active_effects;

		friend class util::componentized<health_system>;

	public:
		/* constructor */
		health_system(float hp, float max, float arm, float max_arm);

		/* apply damage, absorbed by armor first */
		void apply_damage(float amount);
		/* restore health up to the maximum */
		void apply_heal(float amount);
		/* restore armor up to the maximum */
		void apply_shield(float amount);
		/* apply damage straight to health, ignoring armor */
		void apply_true_damage(float amount);

		/* add a status effect and run its on-apply hook */
		void add_effect(std::unique_ptr<combat::status_effect> effect, engine::actor& owner);
		/* tick active effects and drop expired ones */
		void process_effects(float dt, engine::actor& owner);

		/* whether health has dropped to zero */
		bool is_dead() const;
	};
}
