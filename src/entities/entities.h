#ifndef ENGINE_ENTITIES_H
#define ENGINE_ENTITIES

#include <vector>
#include "math/fxpt.h"
#include "engine/actor.h"
#include "engine/projectile.h"
#include "combat/combat.h"
#include "util/indexed-storage.h"
#include "engine/entity.h"

namespace engine::entities
{
	class monster : public actor
	{
	public:
		math::fxpt speed;
		math::fxpt attack_range;
		math::fxpt detection radius;
		util::indexed_storage<entity>::id_t target;
		void update(math::fxpt dt) override;
		void take_damage();
	};

	class monster_basic : public monster {};
	class monster_ranged : public monster {};
	class monster_assault : public monster {};
	class monster_sniper : public monster {};
	class monster_trapper : public monster {};
	class monster_Duzy_Gruby : public monster {};
	class monster_Maly_Szybki : public monster {};
	class monster_spawner : public monster {};
	class monster_magic : public monster {};
	class monster_boss : public monster {};	

	class plasma_projectile : public projectile
	{
	public:
		math::fxpt speed
		int damage;
		
	};
	
	class grenade_projectile : public projectile
	{
	public:
		math::fxpt fuse time;
		mth::fxpt explosion_radius;
	};

	class slug_projectile : public projectile {};

	class player : public actor
	{
	public:
	math::fxpt movement speed;
	mth::fxpt sensitivity

	systems::ammo_inventory inventory;
	std::vector<combat::weapon*> weapons;
	combat::weapon* current weapon;
	int current_weapon_index

	void update(math::fxpt dt) override;
	void move(math::vec3 direction);
	void rotate(math::fxpt yaw, math::fxpt pitch);
	void shoot();
	void switch_weapons(int index);
	void use_grenade();

	void take_damage() override;	
	};

}

#endif
