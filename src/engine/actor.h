#pragma once

#include "renderable-entity.h"
#include "systems/health_systems.h"
#include "util/componentized.h"

namespace combat { class charmed; class slowed; }

namespace engine {

	enum class faction {
		player,
		enemy,
		neutral
	};

	class actor : public renderable_entity, public util::componentized<actor> {
	public:
		using util::componentized<actor>::operator();
		using rendering::sprite::operator();
	protected:
		/* current actor health */
		[[=util::component_field{}]] systems::health_system health;
		/* movement */
		float	  movement_speed;
		/* player / enemy / neither */
		faction	  team;

		friend class util::componentized<actor>;
		friend class combat::charmed;
		friend class combat::slowed;
		friend class projectile;

	public:
		/* constructor */
		actor(math::vec2 const p, float const z, assets::texture_id const tex,
			float const is, float const hp, float const shield,
			float const move_speed, faction const this_team);

		/* take damage */
		virtual void take_damage(float const dmg);
		/* take damage passing through shield */
		virtual void take_true_damage(float const dmg);
		/* heal */
		virtual void heal(float const amount);
		/* restore shield */
		virtual void add_shield(float const amount);
		/* apply an affect */
		virtual void add_effect(std::unique_ptr<combat::status_effect> effect);

		/* whether or not the actor is dead */
		bool is_dead() const;

		/* entity tick */
		virtual void update(float dt) override;
	};
}

