#pragma once

#include "firing-mode.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace engine { class world; }

namespace combat {
    namespace weapons {
        class hitscan_firing_mode : public firing_mode {
            geometry::map_data const& map;
            engine::world const&      world_ref;
            float               max_range;
            float               hit_radius;

        public:
            hitscan_firing_mode(geometry::map_data const& map,
                            engine::world const& world,
                            float range  = 8192.0f,
                            float radius = 32.0f);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
