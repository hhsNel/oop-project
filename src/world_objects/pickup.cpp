#include "pickup.h"
#include "entities/player.h"
#include "rendering/sprite.h"
#include "geometry/map-data.h"

namespace world_object {

pickup::pickup(math::vec2 position, float z, assets::texture_id tex,
               entities::player& pl, geometry::map_data& md,
               util::indexed_storage<geometry::subsector>::id_t sub_id,
               float radius, float scale)
    : renderable_entity(position, z, tex, scale),
      pickup_radius(radius), player_ref(pl), map_ref(md), subsector_id(sub_id) {}

bool pickup::is_consumed() const { return consumed; }

void pickup::update(float) {
    if (consumed) return;

    auto* spr = dynamic_cast<rendering::sprite*>(&player_ref);
    if (!spr) return;

    math::vec2 diff = (*spr)("pos"_f) - (*this)("pos"_f);
    if (diff.sqr_len() >= pickup_radius * pickup_radius) return;

    on_pickup(player_ref);
    map_ref.subsectors[subsector_id].remove_sprite(this);
    consumed = true;
}

}
