#include <iostream>
#include <string>
#include <memory>

#include "entities/player.h"
#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/firing-mode.h"

namespace {
	struct noop_firing_mode : combat::weapons::firing_mode {
		void spawn_bullet(math::vec2, float, float, std::span<engine::actor*>) override {}
	};
}

static constexpr math::vec2 ORIGIN{0.0f, 0.0f};
static constexpr assets::texture_id DUMMY_TEX = 0;

static void result(float v) {
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(int v) {
	std::cout << "RESULT " << v << std::endl;
}
static void result(const std::string& s) {
	std::cout << "RESULT " << s << std::endl;
}

// Exposes protected health data for assertions — lives only in test code.
template<typename M>
struct inspect : public M {
	using M::M;
	float hp()    const { return this->health("current_hp"_f); }
	float armor() const { return this->health("armor"_f); }
	bool  dead()  const { return this->health.is_dead(); }
};

static auto make_owned_pistol() {
	return std::make_unique<combat::weapons::pistol>(std::make_unique<noop_firing_mode>());
}

static auto make_owned_smg() {
	return std::make_unique<combat::weapons::smg>(std::make_unique<noop_firing_mode>());
}

static entities::player make_p() {
	return entities::player(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 0.0f, 2.0f, 1.0f);
}

int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// switch_index — add pistol+smg, switch to index 1 -> current_weapon_index=1
		if (cmd == "switch_index") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.weapons.push_back(make_owned_smg());
			p.switch_weapons(0);
			p.switch_weapons(1);
			result(p.current_weapon_index);

		// switch_back — switch to 1 then back to 0 -> current_weapon_index=0
		} else if (cmd == "switch_back") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.weapons.push_back(make_owned_smg());
			p.switch_weapons(1);
			p.switch_weapons(0);
			result(p.current_weapon_index);

		// switch_oob — switch to index beyond end; index unchanged
		} else if (cmd == "switch_oob") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			p.switch_weapons(5);
			result(p.current_weapon_index);

		// switch_neg — switch to negative index; index unchanged
		} else if (cmd == "switch_neg") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			p.switch_weapons(-1);
			result(p.current_weapon_index);

		// shoot_ammo — pistol mag=8, shoot once -> ammo=7
		} else if (cmd == "shoot_ammo") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			p.shoot();
			result(p.weapons[0]->ammo_count);

		// shoot_no_weapon — no weapon selected, shoot does nothing
		} else if (cmd == "shoot_no_weapon") {
			auto p = make_p();
			p.shoot();
			result("OK");

		// shoot_blocked — rate=2.0: shoot twice without update, second shot blocked -> ammo=7
		} else if (cmd == "shoot_blocked") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			p.shoot();
			p.shoot();
			result(p.weapons[0]->ammo_count);

		// shoot_after_update — rate=2.0: shoot, update 0.6s, shoot -> ammo=6
		} else if (cmd == "shoot_after_update") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			p.shoot();
			p.update(0.6f);
			p.shoot();
			result(p.weapons[0]->ammo_count);

		// reload_current — fire empty (8 shots), reload -> ammo=8
		} else if (cmd == "reload_current") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			for (int i = 0; i < 8; ++i) { p.shoot(); p.update(1.0f); }
			p.reload();
			result(p.weapons[0]->ammo_count);

		// can_fire_after_empty — fire empty, can player still shoot? NO
		} else if (cmd == "can_fire_after_empty") {
			auto p = make_p();
			p.weapons.push_back(make_owned_pistol());
			p.switch_weapons(0);
			for (int i = 0; i < 8; ++i) { p.shoot(); p.update(1.0f); }
			p.shoot(); // should do nothing
			result(p.weapons[0]->ammo_count);

		// =====================================================================
		// HEALTH SYSTEM
		// =====================================================================

		// player_take_damage — player(100 hp, 0 armor), take 20 -> hp=80
		} else if (cmd == "player_take_damage") {
			inspect<entities::player> p(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 0.0f, 2.0f, 1.0f);
			p.take_damage(20.0f);
			result(p.hp());

		// player_armor_absorb — player(100 hp, 50 armor), take 30 -> armor=20, hp=100
		} else if (cmd == "player_armor_absorb") {
			inspect<entities::player> p(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 50.0f, 2.0f, 1.0f);
			p.take_damage(30.0f);
			result(p.armor());
			result(p.hp());

		// player_armor_overflow — player(100 hp, 20 armor), take 30 -> armor=0, hp=90
		} else if (cmd == "player_armor_overflow") {
			inspect<entities::player> p(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 20.0f, 2.0f, 1.0f);
			p.take_damage(30.0f);
			result(p.armor());
			result(p.hp());

		// player_heal — take 20, heal 10 -> hp=90
		} else if (cmd == "player_heal") {
			inspect<entities::player> p(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 0.0f, 2.0f, 1.0f);
			p.take_damage(20.0f);
			p.heal(10.0f);
			result(p.hp());

		// player_death — take 200 -> dead
		} else if (cmd == "player_death") {
			inspect<entities::player> p(ORIGIN, 0.0f, DUMMY_TEX, 1.0f, 100.0f, 0.0f, 2.0f, 1.0f);
			p.take_damage(200.0f);
			result(p.dead() ? "YES" : "NO");

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
