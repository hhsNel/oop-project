#include "ammo-pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"

namespace world_object {

ammo_pickup::ammo_pickup(math::vec2 pos, combat::weapons::ammo_type t, int amt, float radius)
    : pickup(pos, radius), type(t), amount(amt) {}

void ammo_pickup::on_pickup(entities::player& p) {
    for (combat::weapons::weapon* w : p.weapons) {
        if (!w || !w->accepts_ammo(type)) continue;
        w->resupply(amount);
        break;
    }
    collected = true;
}

}
