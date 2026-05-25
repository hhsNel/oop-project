#include "armor-pickup.h"
#include "entities/player.h"

namespace world_object {

armor_pickup::armor_pickup(math::vec2 pos, float amount, float radius)
    : pickup(pos, radius), armor_amount(amount) {}

void armor_pickup::on_pickup(entities::player& p) {
    p.add_shield(armor_amount);
}

}
