#include "armor-pickup.h"
#include "entities/player.h"

namespace world_object {

armor_pickup::armor_pickup(math::vec2 pos, float amount, entities::player& p, engine::world& w,
                           util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
                           float radius)
    : pickup(pos, p, w, id, radius), armor_amount(amount) {}

void armor_pickup::on_pickup(entities::player& p) {
    p.add_shield(armor_amount);
}

}
