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
		/*Dodanie konstruktora health system*/
			health_system() : current_hp(0), max_hp(0), armor(0), max_armor(0) {}

			health_system(float hp, float max, float arm, float max_arm)
				: current_hp(hp), max_hp(max), armor(arm), max_armor(max_arm) {}

			void apply_damage(float amount)
			{
				if(armor > 0.0f)
				{
					float absorbed = std::min(armor, amount);
					armor -= absorbed;
					amount -=absorbed;
				}
				current_hp = std::max(0.0f, current_hp - amount);
			}

			void apply_heal(float amount)
			{
				current_hp = std::min(max_hp, current_hp + amount);
			}

			void apply_shield(float amount)
			{
				armor = std::min(max_armor, armor + amount);
			}

			// Metoda domenowa — obrazenia ignorujace armor, delegowana z actor::take_true_damage
			void apply_true_damage(float amount)
			{
				current_hp = std::max(0.0f, current_hp - amount);
			}

			bool is_dead() const {return current_hp <= 0.0f;}

			void add_effect(std::unique_ptr<combat::status_effect>effect, engine::actor& owner)
			{
				effect->on_apply(owner);
				active_effects.push_back(std::move(effect));
			}

			void process_effects(float dt, engine::actor& owner)
			{
				for(auto& effect : active_effects)
				{
					bool should_tick = !effect->update(dt);
					if(should_tick)
					{
						effect->affect(owner);
					}
					if(effect->is_expired())
					{
						effect->on_expire(owner);
					}
				}

				active_effects.erase(
					std::remove_if(active_effects.begin(), active_effects.end(),
						[](const auto& e) {return e->is_expired();}),
					active_effects.end()
				);
			}

		};
	}

#endif
