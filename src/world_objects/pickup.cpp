#include "pickup.h"

namespace world_object {

pickup::pickup(math::vec2 pos, float radius)
    : position(pos), pickup_radius(radius), collected(false) {}

void pickup::update(float) {}

bool pickup::in_range(math::vec2 player_pos) const {
    math::vec2 diff = player_pos - position;
    return diff.sqr_len() < pickup_radius * pickup_radius;
}

}
