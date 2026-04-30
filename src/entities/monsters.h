#pragma once

#include "entities.h"

namespace engine::entities {

	class monster_basic : public monster {
	public:
		monster_basic(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_ranged : public monster {
	public:
		monster_ranged(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_assault : public monster {
	public:
		monster_assault(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_sniper : public monster {
	public:
		monster_sniper(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp,shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_trapper : public monster {
	public:
		monster_trapper(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_Duzy_Gruby : public monster {
	public:
		monster_Duzy_Gruby(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_Maly_Szybki : public monster {
	public:
		monster_Maly_Szybki(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_spawner : public monster {
	public:
		monster_spawner(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_magic : public monster {
	public:
		monster_magic(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

	class monster_boss : public monster {
	public:
		monster_boss(float hp, float shield, float movement_speed, float attack_range, float detection_radius)
			: monster(hp, shield, movement_speed, attack_range, detection_radius) {}
	};

}


