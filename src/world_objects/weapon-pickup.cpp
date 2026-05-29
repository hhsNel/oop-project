#include "weapon-pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"
#include <typeindex>

namespace world_object {

weapon_pickup::weapon_pickup(math::vec2 position, float z, assets::texture_id tex,
                             std::unique_ptr<combat::weapons::weapon> wpn, int weapon_slot,
                             entities::player& pl, geometry::map_data& md,
                             util::indexed_storage<geometry::subsector>::id_t sub_id,
                             float radius, float scale)
    : pickup(position, z, tex, pl, md, sub_id, radius, scale), provided_weapon(std::move(wpn)), slot(weapon_slot) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    p.equip(slot, std::move(provided_weapon));
}

}
