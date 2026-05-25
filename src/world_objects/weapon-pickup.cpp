#include "weapon-pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"
#include <typeindex>

namespace world_object {

weapon_pickup::weapon_pickup(math::vec2 pos, std::unique_ptr<combat::weapons::weapon> w, float radius)
    : pickup(pos, radius), provided_weapon(std::move(w)) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    auto incoming = std::type_index(typeid(*provided_weapon));
    for (auto& w : p.weapons)
        if (w && std::type_index(typeid(*w)) == incoming) return;
    p.weapons.push_back(std::move(provided_weapon));
}

}
