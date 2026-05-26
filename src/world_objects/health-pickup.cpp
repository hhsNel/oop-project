#include "health-pickup.h"
#include "entities/player.h"

namespace world_object {

health_pickup::health_pickup(math::vec2 pos, float amount, entities::player& p, engine::world& w,
                             util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
                             float radius)
    : pickup(pos, p, w, id, radius), heal_amount(amount) {}

void health_pickup::on_pickup(entities::player& p) {
    p.heal(heal_amount);
}

}
