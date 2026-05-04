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

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
