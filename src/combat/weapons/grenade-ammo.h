#pragma once

#include <vector>
#include "ammunition.h"
#include "geometry/map-data.h"
#include "engine/actor.h"

namespace combat::weapons {

    // Grenade ammunition: manages physically simulated grenades.
    // Each throw creates a live_grenade that moves through the map,
    // bounces off solid walls, and detonates after its fuse expires.
    // Explosion deals damage with linear falloff over explosion_radius.
    //
    // Usage:
    //   - populate `targets` with living actors before each frame
    //   - weapon::update(dt) automatically calls grenade_ammunition::update(dt)
    class grenade_ammunition : public ammunition {
    public:
        struct live_grenade {
            math::vec2 position;
            math::vec2 velocity;
            float      fuse_remaining;
            float      explosion_radius;
            float      damage;
        };

        std::vector<live_grenade> active;   // grenades currently in flight
        std::vector<engine::actor*> targets; // populated by the game loop each frame

        grenade_ammunition(geometry::map_data* map        = nullptr,
                           float               fuse_time  = 2.0f,
                           float               radius     = 80.0f,
                           float               throw_speed = 200.0f);

        // Creates a new live grenade thrown from pos in direction angle.
        void spawn_bullet(math::vec2 pos, float angle, float damage) override;

        // Advances all live grenades: movement, wall bouncing, fuse countdown,
        // and AoE explosion. Called automatically by weapon::update(dt).
        void update(float dt) override;

    private:
        geometry::map_data* map;
        float fuse_time;
        float explosion_radius;
        float throw_speed;
        static constexpr float BOUNCE_DAMPING = 0.5f; // fraction of speed kept after bounce

        void explode(live_grenade const& g);
    };

}
