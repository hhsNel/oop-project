#pragma once

#include <memory>
#include <cstdint>
#include "math/vec2.h"
#include "world_objects/pickup.h"

namespace geometry { class map_data; }
namespace engine { class world; }
namespace entities { class player; }
<<<<<<< HEAD
namespace audio { class audio_mixer; }
namespace assets { class asset_manager; }
=======
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361

namespace world_object {
	std::unique_ptr<pickup> make_pickup(std::uint32_t type, std::uint32_t subtype,
	                                    math::vec2 pos, float z,
	                                    entities::player& player,
	                                    geometry::map_data& md,
	                                    util::indexed_storage<geometry::subsector>::id_t sub_id,
<<<<<<< HEAD
	                                    engine::world& world,
	                                    audio::audio_mixer& mixer,
	                                    assets::asset_manager const& am);
=======
	                                    engine::world const& world);
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
}
