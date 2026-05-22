#pragma once

#include "engine/renderable-entity.h"
#include "engine/actor.h"
#include "math/vec2.h"
#include "assets/ids.h"
#include <vector>

namespace geometry { class map_data; }

namespace entities {

    class projectile : public engine::renderable_entity {
    protected:
        math::vec2 velocity;
        float      damage;
        float      speed;
        float      hit_radius;
        float      max_range;
        float      traveled;
        bool       alive;

        geometry::map_data*          map;
        std::vector<engine::actor*>* targets;

        bool check_wall_collision(math::vec2 const& new_pos) const;
        engine::actor* check_actor_collision(math::vec2 const& new_pos) const;

    public:
        projectile(math::vec2 start, float angle, float dmg,
                   float spd, float radius, float range,
                   geometry::map_data* m, std::vector<engine::actor*>* tgts,
                   assets::texture_id tex, float scale);

        void update(float dt) override;
        bool is_alive() const { return alive; }
    };

    class plasma_projectile : public projectile {
    public:
        plasma_projectile(math::vec2 start, float angle, float dmg,
                          geometry::map_data* m, std::vector<engine::actor*>* tgts);
    };

    class slug_projectile : public projectile {
    public:
        slug_projectile(math::vec2 start, float angle, float dmg,
                        geometry::map_data* m, std::vector<engine::actor*>* tgts);
    };

}
