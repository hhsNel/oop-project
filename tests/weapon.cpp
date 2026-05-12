#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/plasma-ammo.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/katana.h"
#include "combat/weapons/bullet.h"

static void result(float v) {
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(int v) {
	std::cout << "RESULT " << v << std::endl;
}
static void result(const std::string& s) {
	std::cout << "RESULT " << s << std::endl;
}

// Helpers — rate=100 bypasses cooldown unless testing cooldown specifically.
static combat::weapons::pistol make_pistol(
	combat::weapons::bullet_ammunition*& ammo_out,
	int mag = 8, int max = 8, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::bullet_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::pistol(std::move(ammo), mag, max, rate);
}

static combat::weapons::smg make_smg(
	combat::weapons::bullet_ammunition*& ammo_out,
	int mag = 30, int max = 30, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::bullet_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::smg(std::move(ammo), mag, max, rate);
}

static combat::weapons::rifle make_rifle(
	combat::weapons::bullet_ammunition*& ammo_out,
	int mag = 20, int max = 20, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::bullet_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::rifle(std::move(ammo), mag, max, rate);
}

static combat::weapons::sniper_rifle make_sniper(
	combat::weapons::bullet_ammunition*& ammo_out,
	int mag = 5, int max = 5, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::bullet_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::sniper_rifle(std::move(ammo), mag, max, rate);
}

static combat::weapons::plasma_gun make_plasma(
	combat::weapons::plasma_ammunition*& ammo_out,
	int mag = 10, int max = 10, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::plasma_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::plasma_gun(std::move(ammo), mag, max, rate);
}

static combat::weapons::shotgun make_shotgun(
	combat::weapons::bullet_ammunition*& ammo_out,
	int mag = 8, int max = 8, float rate = 100.0f)
{
	auto ammo = std::make_unique<combat::weapons::bullet_ammunition>();
	ammo_out = ammo.get();
	return combat::weapons::shotgun(std::move(ammo), mag, max, rate);
}

int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// pistol_fire — fire once: ammo drops 8→7, 1 bullet spawned
		if (cmd == "pistol_fire") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// pistol_empty — fire full magazine (with cooldown bypass), can_fire() = false
		} else if (cmd == "pistol_empty") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(p.can_fire() ? 1 : 0);

		// pistol_overshoot — fire more than magazine, check spawned count capped at 8
		} else if (cmd == "pistol_overshoot") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 12; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(static_cast<int>(a->shot_count()));

		// pistol_reload — fire empty, reload: can_fire() = true, ammo_count = 8
		} else if (cmd == "pistol_reload") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			p.update(1.0f);
			p.reload();
			result(p.can_fire() ? 1 : 0);
			result(p("ammo_count"_f));

		// pistol_cooldown_ready — fire, advance 0.6s (> 0.5s for rate=2.0), can fire
		} else if (cmd == "pistol_cooldown_ready") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.6f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_cooldown_block — fire, advance 0.3s (< 0.5s for rate=2.0), blocked
		} else if (cmd == "pistol_cooldown_block") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.3f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_no_fire_blocked — second fire during cooldown spawns nothing extra
		} else if (cmd == "pistol_no_fire_blocked") {
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.3f);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(static_cast<int>(a->shot_count()));

		// pistol_bullet_pos <px> <py> <angle> — bullet spawned at correct position
		} else if (cmd == "pistol_bullet_pos") {
			float px, py, ang;
			std::cin >> px >> py >> ang;
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			p.fire({px, py}, ang);
			result(a->shot_at(0).pos("x"_f));
			result(a->shot_at(0).pos("y"_f));
			result(a->shot_at(0).angle);

		// =====================================================================
		// PARAMETRIC
		// =====================================================================

		// fire_count <shots> — fire <shots> times (mag=8, rate=100), report ammo and spawned
		} else if (cmd == "fire_count") {
			int shots;
			std::cin >> shots;
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < shots; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(p("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// cooldown_check <rate> <dt> — fire once, advance dt, report can_fire
		} else if (cmd == "cooldown_check") {
			float rate, dt;
			std::cin >> rate >> dt;
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, rate);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(dt);
			result(p.can_fire() ? "YES" : "NO");

		// reload_check <mag_size> — fire empty, reload, report ammo_count
		} else if (cmd == "reload_check") {
			int mag;
			std::cin >> mag;
			combat::weapons::bullet_ammunition* a;
			auto p = make_pistol(a, mag, mag);
			for (int i = 0; i < mag; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			p.reload();
			result(p("ammo_count"_f));

		// =====================================================================
		// SMG
		// =====================================================================

		// smg_fire — fire once: ammo 30→29, 1 bullet spawned
		} else if (cmd == "smg_fire") {
			combat::weapons::bullet_ammunition* a;
			auto s = make_smg(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// =====================================================================
		// RIFLE
		// =====================================================================

		// rifle_fire — fire once: ammo 20→19, 1 bullet spawned
		} else if (cmd == "rifle_fire") {
			combat::weapons::bullet_ammunition* a;
			auto r = make_rifle(a);
			r.fire({0.0f, 0.0f}, 0.0f);
			result(r("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// =====================================================================
		// SNIPER RIFLE
		// =====================================================================

		// sniper_fire — fire once: ammo 5→4, 1 bullet spawned
		} else if (cmd == "sniper_fire") {
			combat::weapons::bullet_ammunition* a;
			auto s = make_sniper(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// =====================================================================
		// PLASMA GUN
		// =====================================================================

		// plasma_fire — fire once: ammo 10→9, 1 plasma spawned
		} else if (cmd == "plasma_fire") {
			combat::weapons::plasma_ammunition* a;
			auto p = make_plasma(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// =====================================================================
		// SHOTGUN
		// =====================================================================

		// shotgun_fire — fire once: ammo 8→7, pellet_count pellets spawned
		} else if (cmd == "shotgun_fire") {
			combat::weapons::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>(a->shot_count()));

		// shotgun_spread — fire at angle 0, report min and max pellet angles
		} else if (cmd == "shotgun_spread") {
			combat::weapons::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float mn = a->shot_at(0).angle, mx = a->shot_at(0).angle;
			for (auto& r : a->all_shots()) {
				mn = std::min(mn, r.angle);
				mx = std::max(mx, r.angle);
			}
			result(mn);
			result(mx);

		// shotgun_pellet_count <shots> — fire <shots> times, report total pellets
		} else if (cmd == "shotgun_pellet_count") {
			int shots;
			std::cin >> shots;
			combat::weapons::bullet_ammunition* a;
			auto s = make_shotgun(a);
			for (int i = 0; i < shots; ++i) { s.fire({0.0f, 0.0f}, 0.0f); s.update(1.0f); }
			result(static_cast<int>(a->shot_count()));

		// =====================================================================
		// KATANA
		// =====================================================================

		// katana_swing — swing once: swing_count=1, immediately blocked
		} else if (cmd == "katana_swing") {
			combat::weapons::katana k;
			k.fire({0.0f, 0.0f}, 0.0f);
			result(k("swing_count"_f));
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown — swing, advance 0.5s (< 1/1.5≈0.667s), blocked
		} else if (cmd == "katana_cooldown") {
			combat::weapons::katana k(1.5f);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.update(0.5f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown_ready — swing, advance 0.7s (> 0.667s), ready
		} else if (cmd == "katana_cooldown_ready") {
			combat::weapons::katana k(1.5f);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.update(0.7f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_swings <n> — swing n times (cooldown bypass), report swing_count
		} else if (cmd == "katana_swings") {
			int n;
			std::cin >> n;
			combat::weapons::katana k;
			for (int i = 0; i < n; ++i) { k.fire({0.0f, 0.0f}, 0.0f); k.update(1.0f); }
			result(k("swing_count"_f));

		// =====================================================================
		// DAMAGE
		// =====================================================================

		// damage_check <weapon> — fire once, report damage stored in spawned record
		// Weapons: pistol(25) smg(10) rifle(35) sniper(100) plasma(75) shotgun(15) katana(50)
		} else if (cmd == "damage_check") {
			std::string w;
			std::cin >> w;
			if (w == "pistol") {
				combat::weapons::bullet_ammunition* a;
				auto p = make_pistol(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage);
			} else if (w == "smg") {
				combat::weapons::bullet_ammunition* a;
				auto s = make_smg(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage);
			} else if (w == "rifle") {
				combat::weapons::bullet_ammunition* a;
				auto r = make_rifle(a);
				r.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage);
			} else if (w == "sniper") {
				combat::weapons::bullet_ammunition* a;
				auto s = make_sniper(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage);
			} else if (w == "plasma") {
				combat::weapons::plasma_ammunition* a;
				auto p = make_plasma(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage);
			} else if (w == "shotgun") {
				combat::weapons::bullet_ammunition* a;
				auto s = make_shotgun(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->shot_at(0).damage); // damage per pellet
			} else if (w == "katana") {
				combat::weapons::katana k;
				result(k("damage"_f));
			} else {
				result("UNKNOWN");
			}

		// shotgun_total_damage — fire once, sum damage of all pellets
		} else if (cmd == "shotgun_total_damage") {
			combat::weapons::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float total = 0.0f;
			for (auto& r : a->all_shots()) total += r.damage;
			result(total);

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
