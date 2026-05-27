#include "health-pickup.h"
#include "entities/player.h"

namespace world_object {

health_pickup::health_pickup(math::vec2 position, float z, assets::texture_id tex,
                             float amount, entities::player& pl,
                             geometry::map_data& md,
                             util::indexed_storage<geometry::subsector>::id_t sub_id,
                             float radius)
    : pickup(position, z, tex, pl, md, sub_id, radius), heal_amount(amount) {}

void health_pickup::on_pickup(entities::player& p) {
    p.heal(heal_amount);
}

}
