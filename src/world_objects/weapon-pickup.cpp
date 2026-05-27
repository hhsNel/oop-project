#include "weapon-pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"
#include <typeindex>

namespace world_object {

weapon_pickup::weapon_pickup(math::vec2 position, float z, assets::texture_id tex,
                             std::unique_ptr<combat::weapons::weapon> wpn,
                             entities::player& pl, geometry::map_data& md,
                             util::indexed_storage<geometry::subsector>::id_t sub_id,
                             float radius)
    : pickup(position, z, tex, pl, md, sub_id, radius), provided_weapon(std::move(wpn)) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    auto incoming = std::type_index(typeid(*provided_weapon));
    for (auto& w : p.weapons)
        if (w && std::type_index(typeid(*w)) == incoming) return;
    p.weapons.push_back(std::move(provided_weapon));
}

}
