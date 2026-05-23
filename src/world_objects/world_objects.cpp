#include "world_objects.h"
#include "entities/entities.h"
#include "combat/weapons/weapon.h"
#include <algorithm>

namespace world_object {

// ── pickup ────────────────────────────────────────────────────────────────────

pickup::pickup(math::vec2 pos, float radius)
    : position(pos), pickup_radius(radius), collected(false) {}

bool pickup::in_range(math::vec2 player_pos) const {
    math::vec2 diff = player_pos - position;
    return diff.sqr_len() < pickup_radius * pickup_radius;
}

// ── health_pickup ─────────────────────────────────────────────────────────────

health_pickup::health_pickup(math::vec2 pos, float amount, float radius)
    : pickup(pos, radius), heal_amount(amount) {}

void health_pickup::on_pickup(entities::player& p) {
    p.heal(heal_amount);
    collected = true;
}

// ── armor_pickup ──────────────────────────────────────────────────────────────

armor_pickup::armor_pickup(math::vec2 pos, float amount, float radius)
    : pickup(pos, radius), armor_amount(amount) {}

void armor_pickup::on_pickup(entities::player& p) {
    p.add_shield(armor_amount);
    collected = true;
}

// ── ammo_pickup ───────────────────────────────────────────────────────────────

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

// ── weapon_pickup ─────────────────────────────────────────────────────────────

weapon_pickup::weapon_pickup(math::vec2 pos, combat::weapons::weapon* w, float radius)
    : pickup(pos, radius), provided_weapon(w) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    // Only add if the player doesn't already carry this weapon
    for (combat::weapons::weapon const* w : p.weapons)
        if (w == provided_weapon) return;
    p.weapons.push_back(provided_weapon);
    collected = true;
}

} // namespace engine::world_object
