#pragma once

#include "projectile.h"

namespace entities {

    class slug_projectile : public projectile {
    public:
        slug_projectile(math::vec2 start, float angle, float dmg,
                        geometry::map_data* m, std::vector<engine::actor*>* tgts);
    };

}
