#ifndef ENGINE_SYSTEMS_H
#define ENGINE_SYSTEMS_H

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
			health_system() : current_hp(0), max_hp(0), armor(0), max_armor(0) {}

			health_system(float hp, float max, float arm, float max_arm)
				: current_hp(hp), max_hp(max), armor(arm), max_armor(max_arm) {}

			void apply_damage(float amount);
			void apply_heal(float amount);
			void apply_shield(float amount);
			void apply_true_damage(float amount);

			bool is_dead() const { return current_hp <= 0.0f; }

			void add_effect(std::unique_ptr<combat::status_effect> effect, engine::actor& owner);
			void process_effects(float dt, engine::actor& owner);

		};
	}

#endif
