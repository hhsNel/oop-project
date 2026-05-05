#include <iostream>
#include <string>

#include "entities/monsters.h"

// Test helper - exposes protected health data; lives only in test code.
template<typename M>
struct inspect : public M {
	using M::M;
	float hp()    const { return this->health.current_hp; }
	float armor() const { return this->health.armor; }
	bool  dead()  const { return this->health.is_dead(); }
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
			inspect<engine::entities::monster_basic> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "ranged_stats") {
			inspect<engine::entities::monster_ranged> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "assault_stats") {
			inspect<engine::entities::monster_assault> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "sniper_stats") {
			inspect<engine::entities::monster_sniper> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "trapper_stats") {
			inspect<engine::entities::monster_trapper> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "duzy_gruby_stats") {
			inspect<engine::entities::monster_Duzy_Gruby> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "maly_szybki_stats") {
			inspect<engine::entities::monster_Maly_Szybki> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "all_rounder_stats") {
			inspect<engine::entities::monster_all_rounder> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "spawner_stats") {
			inspect<engine::entities::monster_spawner> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "magic_stats") {
			inspect<engine::entities::monster_magic> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_tank_stats") {
			inspect<engine::entities::monster_elite_tank> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "elite_swift_stats") {
			inspect<engine::entities::monster_elite_swift> m;
			result(m.hp());
			result(m.armor());

		} else if (cmd == "boss_stats") {
			inspect<engine::entities::monster_boss> m;
			result(m.hp());
			result(m.armor());

		// =====================================================================
		// SPEED / DETECTION — spot-check key types
		// =====================================================================

		} else if (cmd == "duzy_gruby_speed") {
			engine::entities::monster_Duzy_Gruby m;
			result(m.movement_speed);

		} else if (cmd == "maly_szybki_speed") {
			engine::entities::monster_Maly_Szybki m;
			result(m.movement_speed);

		} else if (cmd == "sniper_detection") {
			engine::entities::monster_sniper m;
			result(m.detection_radius);

		// =====================================================================
		// TYPE-SPECIFIC FIELDS
		// =====================================================================

		} else if (cmd == "assault_burst") {
			engine::entities::monster_assault m;
			result(m.burst_size);
			result(m.burst_interval);

		} else if (cmd == "sniper_interval") {
			engine::entities::monster_sniper m;
			result(m.shoot_interval);

		} else if (cmd == "trapper_traps") {
			engine::entities::monster_trapper m;
			result(m.max_traps);

		} else if (cmd == "spawner_fields") {
			engine::entities::monster_spawner m;
			result(m.max_spawns);
			result(m.spawn_interval);

		} else if (cmd == "elite_swift_charge") {
			engine::entities::monster_elite_swift m;
			result(m.charge_speed);

		} else if (cmd == "boss_phases") {
			engine::entities::monster_boss m;
			result(m.phase_count);

		// =====================================================================
		// HEALTH SYSTEM
		// =====================================================================

		// take_damage: basic (hp=30), take 10 -> hp=20
		} else if (cmd == "take_damage") {
			inspect<engine::entities::monster_basic> m;
			m.take_damage(10.0f);
			result(m.hp());

		// armor_absorb: trapper (armor=15, hp=45), take 10 -> armor=5, hp=45
		} else if (cmd == "armor_absorb") {
			inspect<engine::entities::monster_trapper> m;
			m.take_damage(10.0f);
			result(m.armor());
			result(m.hp());

		// armor_overflow: trapper takes 20 -> armor=0, hp=40 (5 przebija przez pancerz)
		} else if (cmd == "armor_overflow") {
			inspect<engine::entities::monster_trapper> m;
			m.take_damage(20.0f);
			result(m.armor());
			result(m.hp());

		// heal: basic takes 10 then heals 5 -> hp=25
		} else if (cmd == "heal") {
			inspect<engine::entities::monster_basic> m;
			m.take_damage(10.0f);
			m.heal(5.0f);
			result(m.hp());

		// death: basic takes 100 -> dead
		} else if (cmd == "death") {
			inspect<engine::entities::monster_basic> m;
			m.take_damage(100.0f);
			result(m.dead() ? "YES" : "NO");

		// =====================================================================
		// AI — ruch i detekcja
		// =====================================================================

		// basic porusza się w stronę celu w zasięgu detekcji
		} else if (cmd == "basic_moves") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_basic m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f);
			result(m.position.x > 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel poza detection_radius=8
		} else if (cmd == "basic_no_detect") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {10.0f, 0.0f};
			engine::entities::monster_basic m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f);
			result(m.position.x == 0.0f ? "YES" : "NO");

		// basic NIE rusza się gdy cel martwy
		} else if (cmd == "basic_dead_target") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_basic m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			t->take_damage(200.0f);
			m.update(1.0f);
			result(m.position.x == 0.0f ? "YES" : "NO");

		// ranged oddala się gdy cel za blisko (< preferred_dist=8)
		} else if (cmd == "ranged_retreats") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {3.0f, 0.0f};
			engine::entities::monster_ranged m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f);
			result(m.position.x < 0.0f ? "YES" : "NO");

		// ranged przybliża się gdy cel za daleko (> preferred_dist, w detection)
		} else if (cmd == "ranged_advances") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {12.0f, 0.0f};
			engine::entities::monster_ranged m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f);
			result(m.position.x > 0.0f ? "YES" : "NO");

		// maly_szybki po detection ustawia dash i pędzi w stronę celu
		} else if (cmd == "maly_szybki_dashes") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_Maly_Szybki m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(0.01f); // ustawia is_dashing=true
			m.update(0.5f);  // pędzi
			result(m.position.x > 0.0f ? "YES" : "NO");

		// elite_swift krąży wokół celu zmieniając pozycję
		} else if (cmd == "elite_swift_circles") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {0.0f, 0.0f};
			engine::entities::monster_elite_swift m;
			m.position = {5.0f, 0.0f};
			m.charge_cd = 999.0f;
			m.set_target(t);
			m.update(0.5f);
			result(m.position.x < 0.0f ? "YES" : "NO"); // cos(7*0.5)=cos(3.5) < 0

		// =====================================================================
		// AI — ataki
		// =====================================================================

		// basic atakuje cel w zasięgu attack_range — cel traci HP
		} else if (cmd == "basic_attacks") {
			auto t = std::make_shared<inspect<engine::entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {1.0f, 0.0f};
			engine::entities::monster_basic m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f);
			result(t->hp());

		// basic nie atakuje dwa razy pod rząd — cooldown blokuje drugi atak
		} else if (cmd == "basic_attack_cooldown") {
			auto t = std::make_shared<inspect<engine::entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {1.0f, 0.0f};
			engine::entities::monster_basic m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(0.01f); // pierwszy atak
			m.update(0.01f); // cooldown aktywny
			result(t->hp());

		// sniper nie strzela przed upływem shoot_interval=3s
		} else if (cmd == "sniper_no_early") {
			auto t = std::make_shared<inspect<engine::entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_sniper m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(1.0f); // aim_timer=1.0 < shoot_interval=3.0
			result(t->hp());

		// sniper strzela gdy aim_timer >= shoot_interval (30 dmg)
		} else if (cmd == "sniper_fires") {
			auto t = std::make_shared<inspect<engine::entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_sniper m;
			m.position = {0.0f, 0.0f};
			m.aim_timer = 2.99f;
			m.set_target(t);
			m.update(0.1f); // aim_timer=3.09 >= 3.0 -> strzal
			result(t->hp());

		// magic strzela po naladowaniu charge_time=2s (40 dmg)
		} else if (cmd == "magic_fires") {
			auto t = std::make_shared<inspect<engine::entities::player>>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f};
			engine::entities::monster_magic m;
			m.position = {0.0f, 0.0f};
			m.is_charging = true;
			m.charge_timer = 1.99f;
			m.set_target(t);
			m.update(0.1f); // charge_timer=2.09 >= 2.0 -> strzal
			result(t->hp());

		// =====================================================================
		// AI — zachowania specyficzne
		// =====================================================================

		// all_rounder przechodzi w tryb melee gdy cel jest blisko
		} else if (cmd == "all_rounder_melee") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {2.0f, 0.0f}; // < melee_threshold=2.5
			engine::entities::monster_all_rounder m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
			m.update(0.01f);
			result(m.melee_mode ? "YES" : "NO");

		// spawner co spawn_interval=5s tworzy nowego potwora
		} else if (cmd == "spawner_spawns") {
			engine::entities::monster_spawner m;
			m.update(5.1f);
			result(m.current_spawns);

		// trapper po trap_interval=2s rozstawia pułapkę
		} else if (cmd == "trapper_places_trap") {
			auto t = std::make_shared<engine::entities::player>(100.0f, 0.0f, 2.0f, 1.0f);
			t->position = {5.0f, 0.0f}; // w detection_radius=8
			engine::entities::monster_trapper m;
			m.position = {0.0f, 0.0f};
			m.set_target(t);
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
