#pragma once
#define ENGINE_WORLD_OBJECTS_H

#include "engine/renderable-entity.h"
#include "combat/combat.h"
#include "math/fxpt.h"

namespace engine {
	namespace world_object {
		class pickup : public renderable_entity {
		public:
			virtual void on_pickup() = 0;
			virtual ~pickup() = default;
		};

		class health_pickup : public pickup {
		public:
			unsigned int heal_amount;
			void on_pickup() override;
		};
		
		class ammo_pickup :public pickup {
		public:
			/* nope */
			// combat::ammo_type type;
			unsigned int amount;
			void on_pickup override;
		};
			
		class weapon_pickup : public pickup {
		public:
			unsigned int weapon_id;
			void on_pickup() override;
		};

	}
}

