#include <iostream>
#include <string>

#include "entities/monsters/monster-basic.h"
#include "entities/monsters/monster-ranged.h"
#include "entities/monsters/monster-assault.h"
#include "entities/monsters/monster-sniper.h"
#include "entities/monsters/monster-trapper.h"
#include "entities/monsters/monster-duzy-gruby.h"
#include "entities/monsters/monster-maly-szybki.h"
#include "entities/monsters/monster-all-rounder.h"
#include "entities/monsters/monster-spawner.h"
#include "entities/monsters/monster-magic.h"
#include "entities/monsters/monster-elite-tank.h"
#include "entities/monsters/monster-elite-swift.h"
#include "entities/monsters/monster-boss.h"
#include "entities/player.h"

static constexpr math::vec2 ORIGIN{0.0f, 0.0f};
static constexpr assets::texture_id DUMMY_TEX = 0;

// Test helper - exposes protected/private data for test inspection and setup.
// Health-only inspect, works for any actor subclass.
template<typename M>
struct inspect_hp : public M {
	using M::M;
	float hp()    const { return this->health("current_hp"_f); }
	float armor() const { return this->health("armor"_f); }
	bool  dead()  const { return this->health.is_dead(); }
};

// Monster-specific inspect, adds movement/detection/target access.
template<typename M>
struct inspect : public inspect_hp<M> {
	using inspect_hp<M>::inspect_hp;
	using M::movement_speed;
	using M::detection_radius;
	using M::target_ptr;
};

// Disambiguate componentized operator() for sprite (pos)
template<typename T>
auto& as_sprite(T& m) { return static_cast<util::componentized<rendering::sprite>&>(m); }

// Factory helpers
template<typename M>
inspect<M> make_m(math::vec2 pos = ORIGIN) {
	return inspect<M>(pos, 0.0f);
}

static auto make_player(math::vec2 pos = ORIGIN) {
	return inspect_hp<entities::player>(pos, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 0.0f, 2.0f, 1.0f);
}

static void result(float v) {
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(const std::string& s) {
	std::cout << "RESULT " << s << std::endl;
}

int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// =====================================================================
		// DEFAULT STATS — hp / armor
		// =====================================================================

		if (cmd == "basic_stats") {
			auto m = make_m<entities::monster_basic>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "ranged_stats") {
			auto m = make_m<entities::monster_ranged>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "assault_stats") {
			auto m = make_m<entities::monster_assault>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "sniper_stats") {
			auto m = make_m<entities::monster_sniper>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "trapper_stats") {
			auto m = make_m<entities::monster_trapper>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "duzy_gruby_stats") {
			auto m = make_m<entities::monster_Duzy_Gruby>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "maly_szybki_stats") {
			auto m = make_m<entities::monster_Maly_Szybki>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "all_rounder_stats") {
			auto m = make_m<entities::monster_all_rounder>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "spawner_stats") {
			auto m = make_m<entities::monster_spawner>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "magic_stats") {
			auto m = make_m<entities::monster_magic>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_tank_stats") {
			auto m = make_m<entities::monster_elite_tank>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_swift_stats") {
			auto m = make_m<entities::monster_elite_swift>();
			result(m.hp());
			result(m.armor());

		} else if (cmd == "boss_stats") {
			auto m = make_m<entities::monster_boss>();
			result(m.hp());
			result(m.armor());

		// =====================================================================
		// SPEED / DETECTION — spot-check key types
		// =====================================================================

		} else if (cmd == "duzy_gruby_speed") {
			auto m = make_m<entities::monster_Duzy_Gruby>();
			result(m.movement_speed);

		} else if (cmd == "maly_szybki_speed") {
			auto m = make_m<entities::monster_Maly_Szybki>();
			result(m.movement_speed);

		} else if (cmd == "sniper_detection") {
			auto m = make_m<entities::monster_sniper>();
			result(m.detection_radius);

		// =====================================================================
		// HEALTH SYSTEM
		// =====================================================================

		// take_damage: basic (hp=30), take 10 -> hp=20
		} else if (cmd == "take_damage") {
			auto m = make_m<entities::monster_basic>();
			m.take_damage(10.0f);
			result(m.hp());

		// armor_absorb: trapper (armor=15, hp=45), take 10 -> armor=5, hp=45
		} else if (cmd == "armor_absorb") {
			auto m = make_m<entities::monster_trapper>();
			m.take_damage(10.0f);
			result(m.armor());
			result(m.hp());

		// armor_overflow: trapper takes 20 -> armor=0, hp=40 (5 przebija przez pancerz)
		} else if (cmd == "armor_overflow") {
			auto m = make_m<entities::monster_trapper>();
			m.take_damage(20.0f);
			result(m.armor());
			result(m.hp());

		// heal: basic takes 10 then heals 5 -> hp=25
		} else if (cmd == "heal") {
			auto m = make_m<entities::monster_basic>();
			m.take_damage(10.0f);
			m.heal(5.0f);
			result(m.hp());

		// death: basic takes 100 -> dead
		} else if (cmd == "death") {
			auto m = make_m<entities::monster_basic>();
			m.take_damage(100.0f);
			result(m.dead() ? "YES" : "NO");

		// =====================================================================
		// AI — ruch i detekcja
		// =====================================================================

		// basic porusza się w stronę celu w zasięgu detekcji
		} else if (cmd == "basic_moves") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_basic>();
			m.target_ptr = &t;
			m.update(1.0f);
			result(as_sprite(m)("pos"_f)("x"_f) > 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel poza detection_radius=8
		} else if (cmd == "basic_no_detect") {
			auto t = make_player({10.0f, 0.0f});
			auto m = make_m<entities::monster_basic>();
			m.target_ptr = &t;
			m.update(1.0f);
			result(as_sprite(m)("pos"_f)("x"_f) == 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel martwy
		} else if (cmd == "basic_dead_target") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_basic>();
			m.target_ptr = &t;
			t.take_damage(200.0f);
			m.update(1.0f);
			result(as_sprite(m)("pos"_f)("x"_f) == 0.0f ? "YES" : "NO");

		// ranged oddala się gdy cel za blisko (< preferred_dist=8)
		} else if (cmd == "ranged_retreats") {
			auto t = make_player({3.0f, 0.0f});
			auto m = make_m<entities::monster_ranged>();
			m.target_ptr = &t;
			m.update(1.0f);
			result(as_sprite(m)("pos"_f)("x"_f) < 0.0f ? "YES" : "NO");

		// ranged przybliża się gdy cel za daleko (> preferred_dist, w detection)
		} else if (cmd == "ranged_advances") {
			auto t = make_player({12.0f, 0.0f});
			auto m = make_m<entities::monster_ranged>();
			m.target_ptr = &t;
			m.update(1.0f);
			result(as_sprite(m)("pos"_f)("x"_f) > 0.0f ? "YES" : "NO");

		// maly_szybki po detection ustawia dash i pędzi w stronę celu
		} else if (cmd == "maly_szybki_dashes") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_Maly_Szybki>();
			m.target_ptr = &t;
			m.update(0.01f); // ustawia is_dashing=true
			m.update(0.5f);  // pędzi
			result(as_sprite(m)("pos"_f)("x"_f) > 0.0f ? "YES" : "NO");

		// elite_swift krąży wokół celu zmieniając pozycję
		} else if (cmd == "elite_swift_circles") {
			auto t = make_player();
			auto m = make_m<entities::monster_elite_swift>({5.0f, 0.0f});
			m.target_ptr = &t;
			// Pierwszy update: charge_cd=0 -> zaczyna szarze, charge_timer=0.8
			// Szarza konczy sie po dotarciu lub po charge_timer<=0, ustawia charge_cd=5.0
			m.update(0.01f); // start charge
			m.update(1.0f);  // charge_timer<0 -> koniec szarzy, charge_cd=5.0
			// Teraz charge_cd>0 -> tryb krazenia
			float before_x = as_sprite(m)("pos"_f)("x"_f);
			m.update(0.5f);  // krazy wokol celu
			result(as_sprite(m)("pos"_f)("x"_f) != before_x ? "YES" : "NO");

		// =====================================================================
		// AI — ataki
		// =====================================================================

		// basic atakuje cel w zasięgu attack_range — cel traci HP
		} else if (cmd == "basic_attacks") {
			auto t = make_player({1.0f, 0.0f});
			auto m = make_m<entities::monster_basic>();
			m.target_ptr = &t;
			m.update(1.0f);
			result(t.hp());

		// basic nie atakuje dwa razy pod rząd — cooldown blokuje drugi atak
		} else if (cmd == "basic_attack_cooldown") {
			auto t = make_player({1.0f, 0.0f});
			auto m = make_m<entities::monster_basic>();
			m.target_ptr = &t;
			m.update(0.01f); // pierwszy atak
			m.update(0.01f); // cooldown aktywny
			result(t.hp());

		// sniper nie strzela przed upływem shoot_interval=3s
		} else if (cmd == "sniper_no_early") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_sniper>();
			m.target_ptr = &t;
			m.update(1.0f); // aim_timer=1.0 < shoot_interval=3.0
			result(t.hp());

		// sniper strzela gdy aim_timer >= shoot_interval (30 dmg)
		} else if (cmd == "sniper_fires") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_sniper>();
			m.target_ptr = &t;
			// aim_timer starts at 0, shoot_interval=3.0
			// update(3.1) -> aim_timer=3.1 >= 3.0 -> strzal
			m.update(3.1f);
			result(t.hp());

		// magic strzela po naladowaniu charge_time=2s (40 dmg)
		} else if (cmd == "magic_fires") {
			auto t = make_player({5.0f, 0.0f});
			auto m = make_m<entities::monster_magic>();
			m.target_ptr = &t;
			// Pierwszy update ustawia is_charging=true, charge_timer rośnie
			m.update(0.01f);
			// charge_timer musi osiagnac charge_time=2.0
			m.update(2.0f); // charge_timer=2.01 >= 2.0 -> strzal
			result(t.hp());

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
