#pragma once

#include "renderable-entity.h"
#include "actor.h"
#include "world.h"
#include "math/vec2.h"
#include "combat/status-effect.h"
#include <memory>
#include <functional>

namespace geometry { class map_data; }
namespace combat { namespace weapons { class projectile_firing_mode; } }
namespace entities { class monster; class monster_boss; }

namespace engine {

	class projectile : public renderable_entity {
		friend class combat::weapons::projectile_firing_mode;
		friend class entities::monster;
		friend class entities::monster_boss;
	protected:
		/* unit direction of travel */
		math::vec2 direction;
		/* travel speed */
		float speed;
		/* damage dealt on hit */
		float damage;
		/* seconds left before the projectile despawns */
		float lifetime;
		/* radius used for hit detection */
		float hit_radius;
		/* faction that fired the projectile */
		faction team;
		/* makes the status effect applied on hit, may be null */
		std::function<std::unique_ptr<combat::status_effect>()> on_hit_effect;

		/* world the projectile lives in */
		world* world_ref = nullptr;
		/* map used for wall collision */
		geometry::map_data* map_ref = nullptr;
		/* this projectile's id in the world's entity storage */
		util::indexed_storage<std::unique_ptr<entity>>::id_t self_id = 0;

	public:
		/* constructor */
		projectile(math::vec2 pos, float z, assets::texture_id tex, float scale,
		           math::vec2 dir, float spd, float dmg, float life,
		           faction f, float radius = 8.0f);

		/* entity tick, moves the projectile and resolves hits */
		void update(float dt) override;
	};

}
