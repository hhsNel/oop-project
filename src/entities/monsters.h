#pragma once

#include "entities/entities.h"

namespace engine::entities {

	class monster_basic : public monster {
	public:
		monster_basic()
			: monster(30.0f, 0.0f, 1.5f, 1.5f, 8.0f) {}
	};

	class monster_ranged : public monster {
	public:
		monster_ranged()
			: monster(25.0f, 0.0f, 1.2f, 10.0f, 15.0f) {}
	};

	class monster_assault : public monster {
	public:
		int   burst_size;
		float burst_interval;

		monster_assault()
			: monster(60.0f, 10.0f, 2.0f, 6.0f, 12.0f),
			  burst_size(3), burst_interval(0.2f) {}
	};

	class monster_sniper : public monster {
	public:
		float shoot_interval;

		monster_sniper()
			: monster(40.0f, 0.0f, 1.0f, 25.0f, 30.0f),
			  shoot_interval(3.0f) {}
	};

	class monster_trapper : public monster {
	public:
		int max_traps;

		monster_trapper()
			: monster(45.0f, 15.0f, 1.0f, 3.0f, 8.0f),
			  max_traps(3) {}
	};

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby()
			: monster(200.0f, 50.0f, 0.8f, 2.0f, 6.0f) {}
	};

	class monster_Maly_Szybki : public monster {
	public:
		monster_Maly_Szybki()
			: monster(15.0f, 0.0f, 4.5f, 1.5f, 12.0f) {}
	};

	class monster_all_rounder : public monster {
	public:
		monster_all_rounder()
			: monster(80.0f, 20.0f, 2.0f, 5.0f, 10.0f) {}
	};

	class monster_spawner : public monster {
	public:
		int   max_spawns;
		float spawn_interval;

		monster_spawner()
			: monster(100.0f, 20.0f, 0.8f, 0.0f, 15.0f),
			  max_spawns(5), spawn_interval(5.0f) {}
	};

	class monster_magic : public monster {
	public:
		monster_magic()
			: monster(50.0f, 30.0f, 1.5f, 12.0f, 15.0f) {}
	};

	class monster_elite_tank : public monster {
	public:
		monster_elite_tank()
			: monster(400.0f, 150.0f, 1.2f, 2.5f, 8.0f) {}
	};

	class monster_elite_swift : public monster {
	public:
		float charge_speed;

		monster_elite_swift()
			: monster(60.0f, 0.0f, 7.0f, 2.0f, 20.0f),
			  charge_speed(15.0f) {}
	};

	class monster_boss : public monster {
	public:
		int phase_count;

		monster_boss()
			: monster(1000.0f, 300.0f, 1.5f, 3.0f, 30.0f),
			  phase_count(3) {}
	};

}
