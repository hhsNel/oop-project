#ifndef ENGINE_SYSTEMS_H
#define ENGINE_SYSTEMS_H

#include <vector>
#include <memory>
#include <algorithm>

#include "combat/combat.h"
#include "util/indexed-storage.h"

namespace engine {class actor;}

namespace engine {
	namespace systems {
		class health_system {
		public:
			float current_hp;
			float max_hp;
			float armor;
			float max_armor;

			std::vector<std::unique_ptr<combat::status_effect>> active_effects;

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

			bool is_dead() const {return current_hp <= 0.0f;}

			void add_effect(std::unique_ptr<combat::status_effect>effect, actor& owner)
			{
				effect->on_apply(owner);
				active_effects.push_back(std::move(effect));
			}

			void process_effects(float dt, actor& owner)
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
}

#endif
