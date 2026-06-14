#pragma once

#include <memory>
#include <cstdint>
#include "math/vec2.h"
#include "world_objects/pickup.h"

namespace geometry {
	class map_data;
	class pickup_spawn;
}
namespace engine { class world; }
namespace entities { class player; }
namespace audio { class audio_mixer; }
namespace assets { class asset_manager; }

namespace world_object {
	/* build a pickup instance from a spawn descriptor */
	std::unique_ptr<pickup> make_pickup(
		geometry::pickup_spawn const& ps, entities::player& player,
		geometry::map_data& md,
		util::indexed_storage<geometry::subsector>::id_t sub_id,
		engine::world& world, audio::audio_mixer& mixer,
		assets::asset_manager const& am);
}
