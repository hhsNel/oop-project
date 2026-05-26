#include "weapon-pickup.h"
#include "entities/player.h"
#include "combat/weapons/weapon.h"
#include <typeindex>

namespace world_object {

weapon_pickup::weapon_pickup(math::vec2 pos, std::unique_ptr<combat::weapons::weapon> w,
                             entities::player& p, engine::world& world,
                             util::indexed_storage<std::unique_ptr<engine::entity>>::id_t id,
                             float radius)
    : pickup(pos, p, world, id, radius), provided_weapon(std::move(w)) {}

void weapon_pickup::on_pickup(entities::player& p) {
    if (!provided_weapon) return;
    auto incoming = std::type_index(typeid(*provided_weapon));
    for (auto& w : p.weapons)
        if (w && std::type_index(typeid(*w)) == incoming) return;
    p.weapons.push_back(std::move(provided_weapon));
}

}
