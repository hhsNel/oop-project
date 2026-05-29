#include "pickup-factory.h"
#include "health-pickup.h"
#include "armor-pickup.h"
#include "weapon-pickup.h"
#include "ammo-pickup.h"

#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/katana.h"

#include "geometry/map-data.h"
#include "engine/world.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"

namespace world_object {

	// Sprite IDs (in sprite manifest):
	// 13 = health, 14 = armor
	// 15-21 = weapon pickups (pistol, smg, rifle, shotgun, sniper, plasma, katana)
	// 22-27 = ammo pickups (pistol, smg, rifle, shotgun, sniper, plasma)

	static constexpr assets::texture_id health_sprite = 13;
	static constexpr assets::texture_id armor_sprite  = 14;

	static constexpr assets::texture_id weapon_sprite_base = 15;
	static constexpr assets::texture_id ammo_sprite_base   = 22;

	static std::unique_ptr<pickup> make_weapon_pickup(
		std::uint32_t subtype, math::vec2 pos, float z,
		entities::player& player, geometry::map_data& md,
		util::indexed_storage<geometry::subsector>::id_t sub_id,
		engine::world& world,
		audio::audio_mixer& mixer,
		assets::asset_manager const& am)
	{
		assets::texture_id tex = weapon_sprite_base + static_cast<assets::texture_id>(subtype);
		std::unique_ptr<combat::weapons::weapon> w;

		switch (subtype) {
			case 0: w = std::make_unique<combat::weapons::pistol>(md, world, mixer, am);       break;
			case 1: w = std::make_unique<combat::weapons::smg>(md, world, mixer, am);          break;
			case 2: w = std::make_unique<combat::weapons::rifle>(md, world, mixer, am);        break;
			case 3: w = std::make_unique<combat::weapons::shotgun>(md, world, mixer, am);      break;
			case 4: w = std::make_unique<combat::weapons::sniper_rifle>(md, world, mixer, am); break;
			case 5: w = std::make_unique<combat::weapons::plasma_gun>(md, world, mixer, am);   break;
			case 6: w = std::make_unique<combat::weapons::katana>(md, world, mixer, am);        break;
			default: return nullptr;
		}

		return std::make_unique<weapon_pickup>(pos, z, tex, std::move(w), static_cast<int>(subtype), player, md, sub_id);
	}

	static std::unique_ptr<pickup> make_ammo_pickup(
		std::uint32_t subtype, math::vec2 pos, float z,
		entities::player& player, geometry::map_data& md,
		util::indexed_storage<geometry::subsector>::id_t sub_id)
	{
		assets::texture_id tex = ammo_sprite_base + static_cast<assets::texture_id>(subtype);
		constexpr int default_ammo = 1;

		switch (subtype) {
			case 0: return std::make_unique<ammo_pickup<combat::weapons::pistol>>(pos, z, tex, default_ammo, player, md, sub_id);
			case 1: return std::make_unique<ammo_pickup<combat::weapons::smg>>(pos, z, tex, default_ammo, player, md, sub_id);
			case 2: return std::make_unique<ammo_pickup<combat::weapons::rifle>>(pos, z, tex, default_ammo, player, md, sub_id);
			case 3: return std::make_unique<ammo_pickup<combat::weapons::shotgun>>(pos, z, tex, default_ammo, player, md, sub_id);
			case 4: return std::make_unique<ammo_pickup<combat::weapons::sniper_rifle>>(pos, z, tex, default_ammo, player, md, sub_id);
			case 5: return std::make_unique<ammo_pickup<combat::weapons::plasma_gun>>(pos, z, tex, default_ammo, player, md, sub_id);
			default: return nullptr;
		}
	}

	std::unique_ptr<pickup> make_pickup(std::uint32_t type, std::uint32_t subtype,
	                                    math::vec2 pos, float z,
	                                    entities::player& player,
	                                    geometry::map_data& md,
	                                    util::indexed_storage<geometry::subsector>::id_t sub_id,
	                                    engine::world& world,
	                                    audio::audio_mixer& mixer,
	                                    assets::asset_manager const& am)
	{
		switch (type) {
			case 0: return std::make_unique<health_pickup>(pos, z, health_sprite, 25.0f, player, md, sub_id);
			case 1: return std::make_unique<armor_pickup>(pos, z, armor_sprite, 25.0f, player, md, sub_id);
			case 2: return make_weapon_pickup(subtype, pos, z, player, md, sub_id, world, mixer, am);
			case 3: return make_ammo_pickup(subtype, pos, z, player, md, sub_id);
			default: return nullptr;
		}
	}

}
