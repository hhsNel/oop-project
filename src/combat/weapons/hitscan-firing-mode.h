#pragma once

#include <vector>
#include <utility>
#include "firing-mode.h"
#include "util/componentized.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace combat {
    namespace weapons {
        // Hitscan — natychmiastowy strzal rozwiazywany raycastem.
        // Rzuca promien z pozycji strzalu, zatrzymuje sie na najblizszej solidnej scianie,
        // i zadaje obrazenia najblizszemu aktorowi trafionemu przed ta sciana.
        // Uzycie: ustaw cele przez ("targets"_f) = {...} przed kazdym fire().
        class hitscan_firing_mode final : public firing_mode, public util::componentized<hitscan_firing_mode> {
            friend class util::componentized<hitscan_firing_mode>;

            geometry::map_data* map;
            float               max_range;
            float               hit_radius;   // promien cylindra kolizji aktora (jednostki mapy)
            [[=util::ref_component_field{}]] std::vector<engine::actor*> targets;

        public:
            hitscan_firing_mode(geometry::map_data* map,
                            float range  = 8192.0f,
                            float radius = 16.0f);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
