#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "firing-mode.h"
#include "util/componentized.h"
#include "engine/actor.h"

namespace geometry { class map_data; }
namespace entities { class projectile; }

namespace combat {
    namespace weapons {

        class projectile_firing_mode : public firing_mode, public util::componentized<projectile_firing_mode> {
            friend class util::componentized<projectile_firing_mode>;

        public:
            using factory_fn = std::function<std::unique_ptr<entities::projectile>(
                math::vec2, float, float,
                geometry::map_data*, std::vector<engine::actor*>*)>;

        private:
            geometry::map_data*  map;
            factory_fn           factory;
            [[=util::ref_component_field{}]] std::vector<engine::actor*> targets;
            std::vector<std::unique_ptr<entities::projectile>> active;

        public:
            projectile_firing_mode(geometry::map_data* m, factory_fn fn);

            void spawn_bullet(math::vec2 pos, float angle, float damage) override;
            void update(float dt);

            using util::componentized<projectile_firing_mode>::operator();
        };
    }
}
