#include "weapon-pickup.h"
#include "entities/entities.h"
#include "combat/weapons/weapon.h"

namespace world_object {

weapon_pickup::weapon_pickup(math::vec2 pos, combat::weapons::weapon* w, float radius)
    : pickup(pos, radius), provided_weapon(w) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    for (combat::weapons::weapon const* w : p.weapons)
        if (w == provided_weapon) return;
    p.weapons.push_back(provided_weapon);
    collected = true;
}

}
