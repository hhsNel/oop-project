#include <iostream>
#include <string>

#include "entities/monsters.h"

// Test helper - exposes protected/private data for test inspection and setup.
template<typename M>
struct inspect : public M {
	using M::M;
	float hp()    const { return this->health("current_hp"_f); }
	float armor() const { return this->health("armor"_f); }
	bool  dead()  const { return this->health.is_dead(); }
};

// Per-type test wrappers — promote protected fields to public for assertions.
struct test_assault : inspect<entities::monster_assault> {
	using inspect::inspect;
	using entities::monster_assault::burst_size;
	using entities::monster_assault::burst_interval;
};
struct test_sniper : inspect<entities::monster_sniper> {
	using inspect::inspect;
	using entities::monster_sniper::shoot_interval;
};
struct test_trapper : inspect<entities::monster_trapper> {
	using inspect::inspect;
	using entities::monster_trapper::max_traps;
	using entities::monster_trapper::traps_placed;
};
struct test_spawner : inspect<entities::monster_spawner> {
	using inspect::inspect;
	using entities::monster_spawner::max_spawns;
	using entities::monster_spawner::current_spawns;
	using entities::monster_spawner::spawn_interval;
};
struct test_elite_swift : inspect<entities::monster_elite_swift> {
	using inspect::inspect;
	using entities::monster_elite_swift::charge_speed;
};
struct test_boss : inspect<entities::monster_boss> {
	using inspect::inspect;
	using entities::monster_boss::phase_count;
};
struct test_all_rounder : inspect<entities::monster_all_rounder> {
	using inspect::inspect;
	using entities::monster_all_rounder::melee_mode;
};

static void result(float v) {
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(int v) {
	std::cout << "RESULT " << v << std::endl;
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
			inspect<entities::monster_basic> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "ranged_stats") {
			inspect<entities::monster_ranged> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "assault_stats") {
			inspect<entities::monster_assault> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "sniper_stats") {
			inspect<entities::monster_sniper> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "trapper_stats") {
			inspect<entities::monster_trapper> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "duzy_gruby_stats") {
			inspect<entities::monster_Duzy_Gruby> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "maly_szybki_stats") {
			inspect<entities::monster_Maly_Szybki> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "all_rounder_stats") {
			inspect<entities::monster_all_rounder> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "spawner_stats") {
			inspect<entities::monster_spawner> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "magic_stats") {
			inspect<entities::monster_magic> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_tank_stats") {
			inspect<entities::monster_elite_tank> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_swift_stats") {
			inspect<entities::monster_elite_swift> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "boss_stats") {
			inspect<entities::monster_boss> m;
			result(m.hp());
			result(m.armor());

		// =====================================================================
		// SPEED / DETECTION — spot-check key types
		// =====================================================================

		} else if (cmd == "duzy_gruby_speed") {
			entities::monster_Duzy_Gruby m;
			result(m.movement_speed);

		} else if (cmd == "maly_szybki_speed") {
			entities::monster_Maly_Szybki m;
			result(m.movement_speed);

		} else if (cmd == "sniper_detection") {
			entities::monster_sniper m;
			result(m("detection_radius"_f));

		// =====================================================================
		// TYPE-SPECIFIC FIELDS
		// =====================================================================

		} else if (cmd == "assault_burst") {
			test_assault m;
			result(m.burst_size);
			result(m.burst_interval);

		} else if (cmd == "sniper_interval") {
			test_sniper m;
			result(m.shoot_interval);

		} else if (cmd == "trapper_traps") {
			test_trapper m;
			result(m.max_traps);

		} else if (cmd == "spawner_fields") {
			test_spawner m;
			result(m.max_spawns);
			result(m.spawn_interval);

		} else if (cmd == "elite_swift_charge") {
			test_elite_swift m;
			result(m.charge_speed);

		} else if (cmd == "boss_phases") {
			test_boss m;
			result(m.phase_count);

		// =====================================================================
		// HEALTH SYSTEM
		// =====================================================================

		// take_damage: basic (hp=30), take 10 -> hp=20
		} else if (cmd == "take_damage") {
			inspect<entities::monster_basic> m;
			m.take_damage(10.0f);
			result(m.hp());

		// armor_absorb: trapper (armor=15, hp=45), take 10 -> armor=5, hp=45
		} else if (cmd == "armor_absorb") {
			inspect<entities::monster_trapper> m;
			m.take_damage(10.0f);
			result(m.armor());
			result(m.hp());

		// armor_overflow: trapper takes 20 -> armor=0, hp=40 (5 przebija przez pancerz)
		} else if (cmd == "armor_overflow") {
			inspect<entities::monster_trapper> m;
			m.take_damage(20.0f);
			result(m.armor());
			result(m.hp());

		// heal: basic takes 10 then heals 5 -> hp=25
		} else if (cmd == "heal") {
			inspect<entities::monster_basic> m;
			m.take_damage(10.0f);
			m.heal(5.0f);
			result(m.hp());

		// death: basic takes 100 -> dead
		} else if (cmd == "death") {
			inspect<entities::monster_basic> m;
			m.take_damage(100.0f);
			result(m.dead() ? "YES" : "NO");

		// =====================================================================
		// AI — ruch i detekcja
		// =====================================================================

		// basic porusza się w stronę celu w zasięgu detekcji
		} else if (cmd == "basic_moves") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_basic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f);
			result(m.pos("x"_f) > 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel poza detection_radius=8
		} else if (cmd == "basic_no_detect") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {10.0f, 0.0f};
			entities::monster_basic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f);
			result(m.pos("x"_f) == 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel martwy
		} else if (cmd == "basic_dead_target") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_basic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			t->take_damage(200.0f);
			m.update(1.0f);
			result(m.pos("x"_f) == 0.0f ? "YES" : "NO");

		// ranged oddala się gdy cel za blisko (< preferred_dist=8)
		} else if (cmd == "ranged_retreats") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {3.0f, 0.0f};
			entities::monster_ranged m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f);
			result(m.pos("x"_f) < 0.0f ? "YES" : "NO");

		// ranged przybliża się gdy cel za daleko (> preferred_dist, w detection)
		} else if (cmd == "ranged_advances") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {12.0f, 0.0f};
			entities::monster_ranged m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f);
			result(m.pos("x"_f) > 0.0f ? "YES" : "NO");

		// maly_szybki po detection ustawia dash i pędzi w stronę celu
		} else if (cmd == "maly_szybki_dashes") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_Maly_Szybki m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(0.01f); // ustawia is_dashing=true
			m.update(0.5f);  // pędzi
			result(m.pos("x"_f) > 0.0f ? "YES" : "NO");

		// elite_swift krąży wokół celu zmieniając pozycję
		} else if (cmd == "elite_swift_circles") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {0.0f, 0.0f};
			entities::monster_elite_swift m;
			m.pos = {5.0f, 0.0f};
			m("target_ptr"_f) = t;
			// Pierwszy update: charge_cd=0 -> zaczyna szarze, charge_timer=0.8
			// Szarza konczy sie po dotarciu lub po charge_timer<=0, ustawia charge_cd=5.0
			m.update(0.01f); // start charge
			m.update(1.0f);  // charge_timer<0 -> koniec szarzy, charge_cd=5.0
			// Teraz charge_cd>0 -> tryb krazenia
			float before_x = m.pos("x"_f);
			m.update(0.5f);  // krazy wokol celu
			result(m.pos("x"_f) != before_x ? "YES" : "NO");

		// =====================================================================
		// AI — ataki
		// =====================================================================

		// basic atakuje cel w zasięgu attack_range — cel traci HP
		} else if (cmd == "basic_attacks") {
			auto t = std::make_shared<inspect<entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {1.0f, 0.0f};
			entities::monster_basic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f);
			result(t->hp());

		// basic nie atakuje dwa razy pod rząd — cooldown blokuje drugi atak
		} else if (cmd == "basic_attack_cooldown") {
			auto t = std::make_shared<inspect<entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {1.0f, 0.0f};
			entities::monster_basic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(0.01f); // pierwszy atak
			m.update(0.01f); // cooldown aktywny
			result(t->hp());

		// sniper nie strzela przed upływem shoot_interval=3s
		} else if (cmd == "sniper_no_early") {
			auto t = std::make_shared<inspect<entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_sniper m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(1.0f); // aim_timer=1.0 < shoot_interval=3.0
			result(t->hp());

		// sniper strzela gdy aim_timer >= shoot_interval (30 dmg)
		} else if (cmd == "sniper_fires") {
			auto t = std::make_shared<inspect<entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_sniper m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			// aim_timer starts at 0, shoot_interval=3.0
			// update(3.1) -> aim_timer=3.1 >= 3.0 -> strzal
			m.update(3.1f);
			result(t->hp());

		// magic strzela po naladowaniu charge_time=2s (40 dmg)
		} else if (cmd == "magic_fires") {
			auto t = std::make_shared<inspect<entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f};
			entities::monster_magic m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			// Pierwszy update ustawia is_charging=true, charge_timer rośnie
			m.update(0.01f);
			// charge_timer musi osiagnac charge_time=2.0
			m.update(2.0f); // charge_timer=2.01 >= 2.0 -> strzal
			result(t->hp());

		// =====================================================================
		// AI — zachowania specyficzne
		// =====================================================================

		// all_rounder przechodzi w tryb melee gdy cel jest blisko
		} else if (cmd == "all_rounder_melee") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {2.0f, 0.0f}; // < melee_threshold=2.5
			test_all_rounder m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(0.01f);
			result(m.melee_mode ? "YES" : "NO");

		// spawner co spawn_interval=5s tworzy nowego potwora
		} else if (cmd == "spawner_spawns") {
			test_spawner m;
			m.update(5.1f);
			result(m.current_spawns);

		// trapper po trap_interval=2s rozstawia pułapkę
		} else if (cmd == "trapper_places_trap") {
			auto t = std::make_shared<entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->pos = {5.0f, 0.0f}; // w detection_radius=8
			test_trapper m;
			m.pos = {0.0f, 0.0f};
			m("target_ptr"_f) = t;
			m.update(2.1f);
			result(m.traps_placed);

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
