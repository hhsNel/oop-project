#pragma once

#include "entities/entities.h"

namespace entities {

	class monster_basic : public monster {
	public:
		monster_basic() : monster(30.0f, 0.0f, 1.5f, 1.5f, 8.0f, 10.0f, 1.0f) {}
		void update(float dt) override;
	};

	class monster_ranged : public monster {
		float preferred_dist;
	public:
		monster_ranged() : monster(25.0f, 0.0f, 1.2f, 10.0f, 15.0f, 8.0f, 1.5f),
		                   preferred_dist(8.0f) {}
		void update(float dt) override;
	};

	class monster_assault : public monster, public util::componentized<monster_assault> {
		[[=util::component_field{}]] int   burst_size;
		[[=util::component_field{}]] float burst_interval;
		int   burst_remaining;
		float burst_timer;
		float burst_cooldown;
		float strafe_sign;
		float strafe_timer;

		friend class util::componentized<monster_assault>;
	public:
		using util::componentized<monster_assault>::operator();
		using util::componentized<monster>::operator();

		monster_assault() : monster(60.0f, 10.0f, 2.0f, 6.0f, 12.0f, 7.0f, 0.2f),
		    burst_size(3), burst_interval(0.2f),
		    burst_remaining(0), burst_timer(0.0f), burst_cooldown(0.0f),
		    strafe_sign(1.0f), strafe_timer(0.0f) {}
		void update(float dt) override;
	};

	class monster_sniper : public monster, public util::componentized<monster_sniper> {
		[[=util::component_field{}]] float shoot_interval;
		[[=util::component_field{}]] float aim_timer;

		friend class util::componentized<monster_sniper>;
	public:
		using util::componentized<monster_sniper>::operator();
		using util::componentized<monster>::operator();

		monster_sniper() : monster(40.0f, 0.0f, 1.0f, 25.0f, 30.0f, 30.0f, 3.0f),
		    shoot_interval(3.0f), aim_timer(0.0f) {}
		void update(float dt) override;
	};

	class monster_trapper : public monster, public util::componentized<monster_trapper> {
		[[=util::component_field{}]] int   max_traps;
		[[=util::component_field{}]] int   traps_placed;
		float trap_timer;
		[[=util::component_field{}]] float trap_interval;
		float wander_timer;
		float wander_sign;

		friend class util::componentized<monster_trapper>;
	public:
		using util::componentized<monster_trapper>::operator();
		using util::componentized<monster>::operator();

		monster_trapper() : monster(45.0f, 15.0f, 3.0f, 3.0f, 8.0f, 15.0f),
		    max_traps(3), traps_placed(0),
		    trap_timer(0.0f), trap_interval(2.0f),
		    wander_timer(0.0f), wander_sign(1.0f) {}
		void update(float dt) override;
	};

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby() : monster(200.0f, 50.0f, 0.8f, 2.0f, 6.0f, 25.0f, 2.0f) {}
		void update(float dt) override;
	};

	class monster_Maly_Szybki : public monster {
		bool  is_dashing;
		float dash_timer;
		float dash_cooldown;
	public:
		monster_Maly_Szybki() : monster(15.0f, 0.0f, 4.5f, 1.5f, 12.0f, 12.0f, 0.5f),
		    is_dashing(false), dash_timer(0.0f), dash_cooldown(0.0f) {}
		void update(float dt) override;
	};

	class monster_all_rounder : public monster, public util::componentized<monster_all_rounder> {
		[[=util::component_field{}]] bool  melee_mode;
		float melee_threshold;

		friend class util::componentized<monster_all_rounder>;
	public:
		using util::componentized<monster_all_rounder>::operator();
		using util::componentized<monster>::operator();

		monster_all_rounder() : monster(80.0f, 20.0f, 2.0f, 5.0f, 10.0f, 10.0f, 1.0f),
		    melee_mode(false), melee_threshold(2.5f) {}
		void update(float dt) override;
	};

	class monster_spawner : public monster, public util::componentized<monster_spawner> {
		[[=util::component_field{}]] int   max_spawns;
		[[=util::component_field{}]] int   current_spawns;
		[[=util::component_field{}]] float spawn_interval;
		float spawn_timer;

		friend class util::componentized<monster_spawner>;
	public:
		using util::componentized<monster_spawner>::operator();
		using util::componentized<monster>::operator();

		monster_spawner() : monster(100.0f, 20.0f, 0.0f, 0.0f, 15.0f),
		    max_spawns(5), current_spawns(0),
		    spawn_interval(5.0f), spawn_timer(0.0f) {}
		void update(float dt) override;
	};

	class monster_magic : public monster, public util::componentized<monster_magic> {
		float charge_time;
		[[=util::component_field{}]] float charge_timer;
		[[=util::component_field{}]] bool  is_charging;
		float post_fire_cooldown;
		float sidestep_timer;
		float sidestep_sign;

		friend class util::componentized<monster_magic>;
	public:
		using util::componentized<monster_magic>::operator();
		using util::componentized<monster>::operator();

		monster_magic() : monster(50.0f, 30.0f, 1.5f, 12.0f, 15.0f, 40.0f, 2.0f),
		    charge_time(2.0f), charge_timer(0.0f), is_charging(false),
		    post_fire_cooldown(0.0f), sidestep_timer(0.0f), sidestep_sign(1.0f) {}
		void update(float dt) override;
	};

	class monster_elite_tank : public monster {
		bool  melee_mode;
		float melee_threshold;
		float heavy_timer;
		float heavy_cd;
	public:
		monster_elite_tank() : monster(400.0f, 150.0f, 1.2f, 2.5f, 8.0f, 15.0f, 1.2f),
		    melee_mode(false), melee_threshold(3.0f),
		    heavy_timer(0.0f), heavy_cd(6.0f) {}
		void update(float dt) override;
	};

	class monster_elite_swift : public monster, public util::componentized<monster_elite_swift> {
		[[=util::component_field{}]] float charge_speed;
		bool  is_charging;
		float charge_timer;
		[[=util::component_field{}]] float charge_cd;
		float circle_angle;
		float circle_radius;

		friend class util::componentized<monster_elite_swift>;
	public:
		using util::componentized<monster_elite_swift>::operator();
		using util::componentized<monster>::operator();

		monster_elite_swift() : monster(60.0f, 0.0f, 7.0f, 2.0f, 20.0f, 12.0f, 0.8f),
		    charge_speed(15.0f), is_charging(false),
		    charge_timer(0.0f), charge_cd(0.0f),
		    circle_angle(0.0f), circle_radius(5.0f) {}
		void update(float dt) override;
	};

	class monster_boss : public monster, public util::componentized<monster_boss> {
		[[=util::component_field{}]] int phase_count;

		friend class util::componentized<monster_boss>;
	public:
		using util::componentized<monster_boss>::operator();
		using util::componentized<monster>::operator();

		monster_boss() : monster(1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
		    phase_count(3) {}
	};

}
