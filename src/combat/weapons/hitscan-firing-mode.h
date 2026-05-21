#pragma once

#include <vector>
#include <utility>
#include "firing-mode.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace combat {
    namespace weapons {
        // Hitscan — natychmiastowy strzal rozwiazywany raycastem.
        // Rzuca promien z pozycji strzalu, zatrzymuje sie na najblizszej solidnej scianie,
        // i zadaje obrazenia najblizszemu aktorowi trafionemu przed ta sciana.
        //
        // Uzycie: ustaw cele przez populate_targets() przed kazdym fire().
        class hitscan_firing_mode : public firing_mode {
            geometry::map_data* map;
            float               max_range;
            float               hit_radius;   // promien cylindra kolizji aktora (jednostki mapy)
            std::vector<engine::actor*> targets;

        public:
            hitscan_firing_mode(geometry::map_data* map,
                            float range  = 512.0f,
                            float radius = 16.0f);

            void populate_targets(std::vector<engine::actor*> t) { targets = std::move(t); }

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
