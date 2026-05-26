#include "entities/monster-factory.h"
#include "entities/monsters/monster-all-rounder.h"
#include "entities/monsters/monster-assault.h"
#include "entities/monsters/monster-basic.h"
#include "entities/monsters/monster-boss.h"
#include "entities/monsters/monster-duzy-gruby.h"
#include "entities/monsters/monster-elite-swift.h"
#include "entities/monsters/monster-elite-tank.h"
#include "entities/monsters/monster-magic.h"
#include "entities/monsters/monster-maly-szybki.h"
#include "entities/monsters/monster-ranged.h"
#include "entities/monsters/monster-sniper.h"
#include "entities/monsters/monster-spawner.h"
#include "entities/monsters/monster-trapper.h"

namespace entities {
	std::unique_ptr<monster> make_monster(std::uint32_t type, math::vec2 pos, float z) {
		switch (type) {
			case 0:  return std::make_unique<monster_all_rounder>(pos, z);
			case 1:  return std::make_unique<monster_assault>(pos, z);
			case 2:  return std::make_unique<monster_basic>(pos, z);
			case 3:  return std::make_unique<monster_boss>(pos, z);
			case 4:  return std::make_unique<monster_Duzy_Gruby>(pos, z);
			case 5:  return std::make_unique<monster_elite_swift>(pos, z);
			case 6:  return std::make_unique<monster_elite_tank>(pos, z);
			case 7:  return std::make_unique<monster_magic>(pos, z);
			case 8:  return std::make_unique<monster_Maly_Szybki>(pos, z);
			case 9:  return std::make_unique<monster_ranged>(pos, z);
			case 10: return std::make_unique<monster_sniper>(pos, z);
			case 11: return std::make_unique<monster_spawner>(pos, z);
			case 12: return std::make_unique<monster_trapper>(pos, z);
			default: return nullptr;
		}
	}
}
