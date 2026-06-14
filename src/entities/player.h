#pragma once

#include <vector>
#include <memory>

#include "engine/actor.h"
#include "math/vec2.h"
#include "combat/weapons/weapon.h"
#include "util/componentized.h"

namespace geometry { class map_data; }
namespace engine { class world; }

namespace entities {
	class player : public engine::actor, public util::componentized<player> {
	public:
		using util::componentized<player>::operator();
		using engine::actor::operator();

	protected:
		[[=util::component_field{}]] std::vector<std::unique_ptr<combat::weapons::weapon>> weapons;
		float sensitivity;
		[[=util::component_field{}]] int current_weapon_index;
		geometry::map_data* map_ref = nullptr;
		engine::world* world_ref = nullptr;
		float collision_radius = 16.0f;
		float walk_speed;
		float sprint_speed;

		friend class util::componentized<player>;

	public:

		/* constructor */
		player(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float hp, float shield, float move_speed, float sens, geometry::map_data* map = nullptr, engine::world* world = nullptr);

		/* place a weapon into the given inventory slot */
		void equip(int slot, std::unique_ptr<combat::weapons::weapon> wpn);

		/* entity tick */
		void update(float dt) override;
		/* move the player along a direction */
		void move(math::vec2 direction);
		/* rotate the player's facing by a yaw amount */
		void rotate(float yaw);
		/* fire the currently held weapon */
		void shoot();
		/* reload the currently held weapon */
		void reload();
		/* select the weapon in the given slot */
		void switch_weapons(int index);
		/* begin sprinting */
		void start_sprint();
		/* stop sprinting */
		void stop_sprint();

		/* add ammo to the first held weapon of the given type */
		template<typename WeaponT>
		bool resupply(int amount) {
			for (auto& w : weapons) {
				if (!w) continue;
				if (dynamic_cast<WeaponT*>(&*w)) {
					w->resupply(amount);
					return true;
				}
			}
			return false;
		}
	};
}
