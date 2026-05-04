#pragma once
#define WEAPON_H

#include <algorithm>
#include <memory>

#include "math/vec2.h"
#include "ammunition.h"

namespace engine::combat
{
	class weapon {
	public:
		unsigned int weapon_id;
		std::unique_ptr<ammunition> ammo;
		int ammo_count;
		int max_ammo;
		float fire_rate;
		float last_shot_time;
		float damage;

		virtual bool can_fire() const { return ammo_count > 0 && last_shot_time <= 0.0f; }
		void update(float dt) { last_shot_time = std::max(0.0f, last_shot_time - dt); }

		virtual void fire(math::vec2 const pos, float const angle) = 0;
		virtual void reload() = 0;
		virtual ~weapon() = default;

	protected:
		weapon(unsigned int id, std::unique_ptr<ammunition> ammo_type, int mag, int max, float rate, float dmg)
			: weapon_id(id), ammo(std::move(ammo_type)), ammo_count(mag),
			  max_ammo(max), fire_rate(rate), last_shot_time(0.0f), damage(dmg) {}
	};
}

