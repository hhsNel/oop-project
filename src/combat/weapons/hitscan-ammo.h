#pragma once

#include <vector>
#include "ammunition.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace combat {
    namespace weapons {
        // Hitscan ammunition: resolves bullet hits instantly via raycasting.
        // Casts a ray from the firing position, stops at the nearest solid wall,
        // and applies damage to the closest actor hit before that wall.
        //
        // Usage: populate `targets` with all living actors before calling fire().
        class hitscan_ammunition : public ammunition {
            geometry::map_data* map;
            float               max_range;
            float               hit_radius;   // actor hit-cylinder radius (map units)
        public:
            std::vector<engine::actor*> targets;

            // map     – geometry used for wall occlusion (may be nullptr → no wall check)
            // range   – maximum effective range in map units
            // radius  – actor hit-cylinder radius in map units
            hitscan_ammunition(geometry::map_data* map,
                            float range  = 512.0f,
                            float radius = 16.0f);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
        };
    }
}
