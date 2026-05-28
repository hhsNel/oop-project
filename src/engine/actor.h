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
		[[=util::component_field{}]] systems::health_system health;
		float	  movement_speed;
		faction	  team;

		friend class util::componentized<actor>;
		friend class combat::charmed;
		friend class combat::slowed;
		friend class projectile;

	public:
		actor(math::vec2 const p, float const z, assets::texture_id const tex, float const is, float const hp, float const shield, float const move_speed, faction const this_team);

		virtual void take_damage(float const dmg);
		virtual void take_true_damage(float const dmg);
		virtual void heal(float const amount);
		virtual void add_shield(float const amount);
		virtual void add_effect(std::unique_ptr<combat::status_effect> effect);

		bool is_dead() const;

		virtual void update(float dt) override;
	};
}

