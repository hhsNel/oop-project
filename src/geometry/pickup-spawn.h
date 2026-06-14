#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include "math/vec2.h"
#include "util/resource.h"
#include "util/componentized.h"
#include "world_objects/pickup.h"
#include "assets/asset-manager.h"

namespace entities { class player; }
namespace audio { class audio_mixer; }
namespace engine { class world; }
namespace geometry { class pickup_spawn; }
namespace world_object {
    /* build a pickup instance from a spawn descriptor */
    std::unique_ptr<world_object::pickup> make_pickup(
        geometry::pickup_spawn const& ps, entities::player& player,
        geometry::map_data& md,
        util::indexed_storage<geometry::subsector>::id_t sub_id,
        engine::world& world, audio::audio_mixer& mixer,
        const assets::asset_manager& am);
}

namespace geometry {
	class pickup_spawn : public util::componentized<pickup_spawn> {
		/* pickup category id */
		std::uint32_t type;
		/* variant within the category */
		std::uint32_t subtype;
		/* world position to spawn at */
		[[=util::component_field{}]] math::vec2 pos;
		/* spawn height */
		float z;

		friend class util::componentized<pickup_spawn>;

	public:
		/* constructor */
		pickup_spawn(std::uint32_t t, std::uint32_t st, math::vec2 p, float height);

		/* parse a list of pickup spawns from a binary resource */
		static std::vector<pickup_spawn> load_from_bin(
			util::resource const& res);

		/* lets the factory read this spawn's private fields */
		friend std::unique_ptr<world_object::pickup>
		world_object::make_pickup(
			pickup_spawn const& ps, entities::player& player,
			geometry::map_data& md,
			util::indexed_storage<geometry::subsector>::id_t sub_id,
			engine::world& world, audio::audio_mixer& mixer,
			assets::asset_manager const& am);
	};
}
