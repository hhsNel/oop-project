#pragma once

#include <vector>
#include <cstdint>
#include "math/vec2.h"
#include "util/resource.h"
#include "util/componentized.h"

#include "world_objects/pickup-factory.h"

namespace entities { class player; }
namespace audio { class audio_mixer; }
namespace engine { class world; }
namespace geometry { class pickup_spawn; }
namespace world_object {
    class pickup;
    std::unique_ptr<pickup> make_pickup(const geometry::pickup_spawn& ps, entities::player& player, geometry::map_data& md, util::indexed_storage<geometry::subsector>::id_t sub_id, engine::world& world, audio::audio_mixer& mixer, const assets::asset_manager& am);
}

namespace geometry {
	class pickup_spawn : public util::componentized<pickup_spawn> {
		std::uint32_t type;
		std::uint32_t subtype;
		[[=util::component_field{}]] math::vec2 pos;
		float z;

		friend class util::componentized<pickup_spawn>;

	public:
		pickup_spawn(std::uint32_t t, std::uint32_t st, math::vec2 p, float height);

		static std::vector<pickup_spawn>  load_from_bin(util::resource const& res);

		friend std::unique_ptr<world_object::pickup> world_object::make_pickup(pickup_spawn const& ps, entities::player& player, geometry::map_data& md, util::indexed_storage<geometry::subsector>::id_t sub_id, engine::world& world, audio::audio_mixer& mixer, assets::asset_manager const& am);
	};                                    
}
