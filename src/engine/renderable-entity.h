#pragma once
#include "entity.h"
#include "rendering/sprite.h"

namespace engine {
    class renderable_entity : public entity, public rendering::sprite {
    public:
        renderable_entity(math::vec2 const p, float const z, graphics::texture_set::texture_id const tex, float const is)
            : rendering::sprite(p, z, tex, is) {}

        renderable_entity() : rendering::sprite() {}

        virtual ~renderable_entity() = default;
    };
}

