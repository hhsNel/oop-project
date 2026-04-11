#ifndef ENGINE_COMBAT_H
#define ENGINE_COMBAT_H

#include <vector>
#include "util/resource.h" 
#include "math/fxpt.h"
#include "math/vec3.h"

namespace engine::combat
{
	class status_type
	{
		none,
		burning,
		slowed,
		charmed,
	};

	class ammo_type
	{
		bullet,
		shell,
		plasma,
		grenade
	};

	class status_effect : public util::resource
	{
	public:
		status_type type;
		math::fxpt duration;
		int intensity;
		math::fxpt tick_timer;
	};

	class weapon
	{
	public:
		ammo_type type;
		int ammo_per_shot;
		math::fxpt fire_rate;
		math::fxpt last_shot_time;
		
		virtual void fire(math::vec3 pos, math:: vec3 dir) = 0;
		virtual void reload() = 0;
		virtual ~weapon() = deafault;
	};

	class hitscan_weapon : public weapon
	{
	public:
	int damage;
	math::fxpt spread;

	void fire(math::vec3 pos, math::vec3 dir) override;
	void reload() override {/* Tu logika przeładowania*/}
	};
	
	class projectile_weapon : public weapon
	{
	public:
		void fire(math::vec3 pos, math::vec3 dir) override;
		void reload() override {/*Tu logika przeładowania*/}
	};


}



#endif
