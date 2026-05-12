#include "world_objects.h"
#include "entities/entities.h"
#include "combat/weapons/weapon.h"
#include <algorithm>

namespace world_object {

// ── pickup ────────────────────────────────────────────────────────────────────

bool pickup::in_range(math::vec2 player_pos) const {
    math::vec2 diff = player_pos - position;
    return diff.sqr_len() < pickup_radius * pickup_radius;
}

// ── health_pickup ─────────────────────────────────────────────────────────────

void health_pickup::on_pickup(entities::player& p) {
    p.heal(heal_amount);
    collected = true;
}

// ── armor_pickup ──────────────────────────────────────────────────────────────

void armor_pickup::on_pickup(entities::player& p) {
    p.add_shield(armor_amount);
    collected = true;
}

// ── ammo_pickup ───────────────────────────────────────────────────────────────

/*
Uzycie metod domenowych accepts_ammo/resupply zamiast componentized
do identyfikacji broni i uzupelnienia amunicji
*/
void ammo_pickup::on_pickup(entities::player& p) {
    for (combat::weapons::weapon* w : p.weapons) {
        if (!w || !w->accepts_ammo(weapon_id)) continue;
        w->resupply(static_cast<int>(amount));
        break;
    }
    collected = true;
}

// ── weapon_pickup ─────────────────────────────────────────────────────────────

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    // Only add if the player doesn't already carry this weapon
    for (combat::weapons::weapon const* w : p.weapons)
        if (w == provided_weapon) return;
    p.weapons.push_back(provided_weapon);
    collected = true;
}

} // namespace engine::world_object
