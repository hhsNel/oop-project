#include "grenade.h"

namespace combat {
    namespace weapons {

        grenade_weapon::grenade_weapon(std::unique_ptr<ammunition> ammo_type,
                                    int count, int max_count, float rate, float dmg)
            : weapon(0, std::move(ammo_type), count, max_count, rate, dmg) {}

        void grenade_weapon::fire(math::vec2 pos, float angle) {
            if (!can_fire()) return;
            (*this)("ammo"_f)->spawn_bullet(pos, angle, (*this)("damage"_f));
            --(*this)("ammo_count"_f);
            (*this)("last_shot_time"_f) = 1.0f / (*this)("fire_rate"_f);
        }

        void grenade_weapon::reload() {
            (*this)("ammo_count"_f) = (*this)("max_ammo"_f);
        }
    }
}
