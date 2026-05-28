#pragma once

#include "entities/monster.h"
#include "engine/world.h"
#include "geometry/map-data.h"
#include <vector>

namespace entities {

	class monster_boss : public monster {
		friend class game::game;
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

		// map ref for boss-specific spawning (set via game::game friend)
		geometry::map_data* boss_map_ref = nullptr;

		float hp_ratio() const;
		void  update_phase();
		void  phase1_ai(float dt, float dist);
		void  phase2_ai(float dt, float dist);
		void  phase3_ai(float dt, float dist);
		void  ranged_attack_slow();
		void  melee_attack_burn();
		void  start_charge();
		void  update_charge(float dt);
		void  start_channel();
		bool  minions_alive() const;

	public:
		monster_boss(math::vec2 const p, float const z);

		bool channeling() const { return is_channeling; }
		float flash_phase() const { return flash_timer; }

		void update(float dt) override;
	};

}
