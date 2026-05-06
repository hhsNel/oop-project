#include "grenade.h"

namespace combat {
    namespace weapons {

        grenade_weapon::grenade_weapon(std::unique_ptr<ammunition> ammo_type,
                                    int count, int max_count, float rate, float dmg)
            : weapon(0, std::move(ammo_type), count, max_count, rate, dmg) {}

        void grenade_weapon::fire(math::vec2 pos, float angle) {
            if (!can_fire()) return;
            ammo->spawn_bullet(pos, angle, damage);
            --ammo_count;
            last_shot_time = 1.0f / fire_rate;
        }

        void grenade_weapon::reload() {
            ammo_count = max_ammo;
        }
    }
}
