#pragma once

#include "entities/monster.h"
#include "engine/world.h"
#include "geometry/map-data.h"
#include <vector>

namespace entities {

	class monster_boss : public monster {
		// phase tracking
		int current_phase = 1;
		float max_hp_val;

		// ranged: 4 hitscans with slow
		float ranged_cooldown = 0.0f;
		static constexpr float ranged_cd_max   = 3.0f;
		static constexpr int   projectile_count = 4;
		static constexpr float projectile_spread = 0.15f;

		// melee: burning
		// uses base attack_cooldown / attack_cd_max

		// charge (phase 2+3)
		bool  is_charging     = false;
		float charge_timer    = 0.0f;
		float charge_cooldown = 0.0f;
		float post_charge_stun = 0.0f;
		math::vec2 charge_dir{0.0f, 0.0f};
		static constexpr float charge_speed_mult = 5.0f;
		static constexpr float charge_duration   = 1.2f;
		static constexpr float charge_cd_max     = 6.0f;
		static constexpr float stun_duration     = 2.0f;

		// channel (phase 3)
		bool  is_channeling   = false;
		float channel_timer   = 0.0f;
		float flash_timer     = 0.0f;
		bool  phase3_debuffed = false;

		// spawned minions tracking
		std::vector<util::indexed_storage<std::unique_ptr<engine::entity>>::id_t> minion_ids;

		// combat stance
		bool  wants_melee    = false;
		float stance_timer   = 10.0f;

		geometry::map_data& boss_map_ref;

		/* current health as a fraction of max health */
		float hp_ratio() const;
		/* advance to the next phase when health thresholds are crossed */
		void  update_phase();
		/* phase 1 behaviour */
		void  phase1_ai(float dt, float dist);
		/* phase 2 behaviour */
		void  phase2_ai(float dt, float dist);
		/* phase 3 behaviour */
		void  phase3_ai(float dt, float dist);
		/* fire a ranged volley that slows the target */
		void  ranged_attack_slow();
		/* melee strike that applies burning */
		void  melee_attack_burn();
		/* begin a charge dash toward the target */
		void  start_charge();
		/* advance an in-progress charge */
		void  update_charge(float dt);
		/* begin the phase 3 channel */
		void  start_channel();
		/* whether any spawned minions are still alive */
		bool  minions_alive() const;

	public:
		/* constructor */
		monster_boss(math::vec2 const p, float const z, engine::actor& target, geometry::map_data& map, engine::world& world);

		/* whether the boss is currently channeling */
		bool channeling() const;
		/* current flash animation phase, for rendering */
		float flash_phase() const;

		/* entity tick */
		void update(float dt) override;
	};

}
