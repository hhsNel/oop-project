#include "pickup.h"
#include "entities/player.h"
#include "rendering/sprite.h"

namespace world_object {

pickup::pickup(math::vec2 pos, entities::player& p, engine::world& w,
               util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
               float radius)
    : position(pos), pickup_radius(radius), player_ref(p), world_ref(w), self_id(id) {}

void pickup::update(float) {
    auto* spr = dynamic_cast<rendering::sprite*>(&player_ref);
    if (spr && in_range((*spr)("pos"_f))) {
        on_pickup(player_ref);
        world_ref.delete_entity(self_id);
    }
}

bool pickup::in_range(math::vec2 player_pos) const {
    math::vec2 diff = player_pos - position;
    return diff.sqr_len() < pickup_radius * pickup_radius;
}

}
