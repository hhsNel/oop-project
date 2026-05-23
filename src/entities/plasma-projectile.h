#pragma once

#include "projectile.h"

namespace entities {

    class plasma_projectile : public projectile {
    public:
        plasma_projectile(math::vec2 start, float angle, float dmg,
                          geometry::map_data* m, std::vector<engine::actor*>* tgts);
    };

}
