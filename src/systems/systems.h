#ifndef ENGINE_SYSTEMS_H
#define ENGINE_SYSTEMS_H

#include <vector>
#include <map>
#include "math/fxpt.h"
#include "engine/combat.h"

namespace engine::systems
{
	class health_system
	{
	public:
	int current_hp;
	int max_hp;
	int armor;

	std::vector<combat::status_effect> active_effects;

	void apply_damage(int amount, combat::status_type type);
	void apply_heal(int amount);

	void process_effects(math::fxpt dt)
		
	};


	class ammo_inventory
	{
	public:
		std::map<combat::ammo_type, int> ammo_counts;

		int get_ammo(combat::ammo_type type);
		void add_ammo(combat::ammo_type type, int amount);

		bool consume_ammo(combat::ammo_type type, int amount);
	};


}

#endif
