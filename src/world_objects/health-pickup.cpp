#include "health-pickup.h"
#include "entities/entities.h"

namespace world_object {

health_pickup::health_pickup(math::vec2 pos, float amount, float radius)
    : pickup(pos, radius), heal_amount(amount) {}

void health_pickup::on_pickup(entities::player& p) {
    p.heal(heal_amount);
    collected = true;
}

}
