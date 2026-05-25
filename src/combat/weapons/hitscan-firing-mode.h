#pragma once

#include "firing-mode.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

int main();

namespace engine { class world; }

namespace combat {
    namespace weapons {
        // Hitscan — natychmiastowy strzal rozwiazywany raycastem.
        // Rzuca promien z pozycji strzalu, zatrzymuje sie na najblizszej solidnej scianie,
        // i zadaje obrazenia najblizszemu aktorowi trafionemu przed ta sciana.
        class hitscan_firing_mode : public firing_mode {
            geometry::map_data const* map;
            engine::world const*      world_ref;
            float               max_range;
            float               hit_radius;   // promien cylindra kolizji aktora (jednostki mapy)

            friend int ::main();

        public:
            hitscan_firing_mode(geometry::map_data const* map = nullptr,
                            engine::world const* world = nullptr,
                            float range  = 8192.0f,
                            float radius = 16.0f);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
