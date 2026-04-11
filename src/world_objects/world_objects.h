#ifndef ENGINE_WORLD_OBJECTS_H
#define ENGINE_WORLD_OBJECTS_H

#include "engine/renderable-entity.h"
#include "combat/combat.h"
#include "math/fxpt.h"

namespace::world_object
{
	class pickup : public renderable_entity 
	{
	public:
		virtual void on_pickup() = 0;
		virtual ~pickup() = default;
	};

	class health_pickup : public pickup
	{
	public:
		int heal_amount;
		void on_pickup() override;
	};
	
	class ammo_pickup :public pickup
	{
	public:
		combat::ammo_type type;
		int amount;
		void on_pickup override;
	};
        
	class weapon_pickup : public pickup
	{
	public:
		int weapon_id;
		void on_pickup() override;
	};

}

#endif
