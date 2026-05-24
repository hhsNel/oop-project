#pragma once

#include <vector>
#include "firing-mode.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

int main();

namespace combat {
    namespace weapons {
        // Hitscan — natychmiastowy strzal rozwiazywany raycastem.
        // Rzuca promien z pozycji strzalu, zatrzymuje sie na najblizszej solidnej scianie,
        // i zadaje obrazenia najblizszemu aktorowi trafionemu przed ta sciana.
        class hitscan_firing_mode : public firing_mode {
            geometry::map_data const* map;
            float               max_range;
            float               hit_radius;   // promien cylindra kolizji aktora (jednostki mapy)
            std::vector<engine::actor*> targets;

            friend int ::main();

        public:
            hitscan_firing_mode(geometry::map_data const* map,
                            float range  = 8192.0f,
                            float radius = 16.0f);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
