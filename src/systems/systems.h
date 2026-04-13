#ifndef ENGINE_SYSTEMS_H
#define ENGINE_SYSTEMS_H

#include <vector>
#include <map>
#include "math/fxpt.h"
#include "engine/combat.h"
#include "util/indexed-storage.h"

namespace engine {
	namespace systems {
		class health_system {
		public:
			int current_hp;
			int max_hp;
			int armor;

			util::indexed_storage< std::unique_ptr<combat::status_effect> > active_effects;

			void apply_damage(unsigned int amount, combat::status_type type);
			void apply_heal(unsigned int amount);
			void apply_shield(unsigned int amount);

			void process_effects(math::fxpt dt)
		};


		/* TODO: move this to engine::combat::weapon */
		//class ammo_inventory {
		//public:

		//	unsigned int get_ammo(combat::ammo_type type) const;
		//	void add_ammo(combat::ammo_type type, int amount);

		//	bool consume_ammo(combat::ammo_type type, int amount);
		//};
	}
}

#endif
