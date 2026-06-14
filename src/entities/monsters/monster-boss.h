#pragma once

#include "entities/monster.h"
#include "engine/world.h"
#include "geometry/map-data.h"
#include <vector>

namespace entities {

	class monster_boss : public monster {
		/* current phase number (1-3) */
		int current_phase = 1;
		/* health the boss started with, used for phase thresholds */
		float max_hp_val;

		/* time until the next ranged volley */
		float ranged_cooldown = 0.0f;
		/* cooldown between ranged volleys */
		static constexpr float ranged_cd_max = 3.0f;
		/* number of projectiles per ranged volley */
		static constexpr int projectile_count = 4;
		/* angular spread of the ranged volley, in radians */
		static constexpr float projectile_spread = 0.15f;

		/* whether the boss is currently charging */
		bool is_charging = false;
		/* time left in the current charge */
		float charge_timer = 0.0f;
		/* time until the next charge is available */
		float charge_cooldown = 0.0f;
		/* stun time left after a charge ends */
		float post_charge_stun = 0.0f;
		/* direction of the current charge */
		math::vec2 charge_dir{0.0f, 0.0f};
		/* speed multiplier applied while charging */
		static constexpr float charge_speed_mult = 5.0f;
		/* how long a charge lasts */
		static constexpr float charge_duration = 1.2f;
		/* cooldown between charges */
		static constexpr float charge_cd_max = 6.0f;
		/* how long the post-charge stun lasts */
		static constexpr float stun_duration = 2.0f;

		/* whether the boss is channeling its phase 3 ability */
		bool is_channeling = false;
		/* time spent in the current channel */
		float channel_timer = 0.0f;
		/* timer driving the channel flash effect */
		float flash_timer = 0.0f;
		/* whether the phase 3 debuff has been applied */
		bool phase3_debuffed = false;

		/* ids of the minions spawned by the boss */
		std::vector<
			util::indexed_storage<std::unique_ptr<engine::entity>>::id_t
		> minion_ids;

		/* whether the boss currently prefers melee */
		bool wants_melee = false;
		/* time until the combat stance is re-evaluated */
		float stance_timer = 10.0f;

		/* map used for charge collision */
		geometry::map_data& boss_map_ref;

		/* current health as a fraction of max health */
		float hp_ratio() const;
		/* advance to the next phase when health thresholds are crossed */
		void update_phase();
		/* phase 1 behaviour */
		void phase1_ai(float dt, float dist);
		/* phase 2 behaviour */
		void phase2_ai(float dt, float dist);
		/* phase 3 behaviour */
		void phase3_ai(float dt, float dist);
		/* fire a ranged volley that slows the target */
		void ranged_attack_slow();
		/* melee strike that applies burning */
		void melee_attack_burn();
		/* begin a charge dash toward the target */
		void start_charge();
		/* advance an in-progress charge */
		void update_charge(float dt);
		/* begin the phase 3 channel */
		void start_channel();
		/* whether any spawned minions are still alive */
		bool minions_alive() const;

	public:
		/* constructor */
		monster_boss(math::vec2 const p, float const z,
			engine::actor& target, geometry::map_data& map,
			engine::world& world);

		/* whether the boss is currently channeling */
		bool channeling() const;
		/* current flash animation phase, for rendering */
		float flash_phase() const;

		/* entity tick */
		void update(float dt) override;
	};

}
