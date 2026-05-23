#include "projectile-firing-mode.h"
#include "entities/projectile.h"
#include "geometry/map-data.h"

namespace combat {
    namespace weapons {

        projectile_firing_mode::projectile_firing_mode(geometry::map_data* m, factory_fn fn)
            : map(m), factory(std::move(fn)) {}

        void projectile_firing_mode::spawn_bullet(math::vec2 pos, float angle, float damage) {
            auto proj = factory(pos, angle, damage, map, &targets);
            if (proj) {
                auto ss_id = map->get_subsector_id(pos);
                map->subsectors[ss_id].sprites.push_back(
                    std::unique_ptr<rendering::sprite>(static_cast<rendering::sprite*>(proj.get()))
                );
                active.push_back(std::move(proj));
            }
        }

        void projectile_firing_mode::update(float dt) {
            for (auto& proj : active) {
                if (proj && proj->is_alive())
                    proj->update(dt);
            }

            // Remove dead projectiles — release sprite from subsector first
            std::erase_if(active, [this](std::unique_ptr<entities::projectile>& proj) {
                if (!proj || !proj->is_alive()) {
                    if (proj && map) {
                        auto ss_id = map->get_subsector_id(proj->pos);
                        auto& sprites = map->subsectors[ss_id].sprites;
                        auto it = std::find_if(sprites.begin(), sprites.end(),
                            [&](std::unique_ptr<rendering::sprite> const& s) {
                                return s.get() == static_cast<rendering::sprite*>(proj.get());
                            });
                        if (it != sprites.end()) {
                            it->release();
                            sprites.erase(it);
                        }
                    }
                    return true;
                }
                return false;
            });
        }

    }
}
