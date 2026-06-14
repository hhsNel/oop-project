#pragma once

#include "firing-mode.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace engine { class world; }

namespace combat {
    namespace weapons {
        class hitscan_firing_mode : public firing_mode {
            /* map used for wall collision */
            geometry::map_data const& map;
            /* world queried for actors to hit */
            engine::world const& world_ref;
            /* maximum trace distance */
            float max_range;
            /* radius used for actor hit tests */
            float hit_radius;

        public:
            /* constructor */
            hitscan_firing_mode(geometry::map_data const& map,
                            engine::world const& world,
                            float range  = 8192.0f,
                            float radius = 32.0f);

            /* trace an instant ray and damage the first actor hit */
            void spawn_bullet(math::vec2 pos, float angle,
                              float damage) override;
        };
    }
}
