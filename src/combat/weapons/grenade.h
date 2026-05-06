#pragma once
#include "weapon.h"

namespace combat::weapons {
    // Grenade launcher / grenade slot.
    // Requires a grenade_ammunition instance as ammo_type.
    class grenade_weapon : public weapon {
    public:
        grenade_weapon(std::unique_ptr<ammunition> ammo_type,
                       int   count     = 3,
                       int   max_count = 3,
                       float rate      = 0.8f,
                       float dmg       = 100.0f);
        void fire(math::vec2 pos, float angle) override;
        void reload() override;
    };
}
