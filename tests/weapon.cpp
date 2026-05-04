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
static engine::combat::pistol make_pistol(
	engine::combat::bullet_ammunition*& ammo_out,
	int mag = 8, int max = 8, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::bullet_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::pistol(std::move(ammo), mag, max, rate);
}

static engine::combat::smg make_smg(
	engine::combat::bullet_ammunition*& ammo_out,
	int mag = 30, int max = 30, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::bullet_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::smg(std::move(ammo), mag, max, rate);
}

static engine::combat::rifle make_rifle(
	engine::combat::bullet_ammunition*& ammo_out,
	int mag = 20, int max = 20, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::bullet_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::rifle(std::move(ammo), mag, max, rate);
}

static engine::combat::sniper_rifle make_sniper(
	engine::combat::bullet_ammunition*& ammo_out,
	int mag = 5, int max = 5, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::bullet_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::sniper_rifle(std::move(ammo), mag, max, rate);
}

static engine::combat::plasma_gun make_plasma(
	engine::combat::plasma_ammunition*& ammo_out,
	int mag = 10, int max = 10, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::plasma_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::plasma_gun(std::move(ammo), mag, max, rate);
}

static engine::combat::shotgun make_shotgun(
	engine::combat::bullet_ammunition*& ammo_out,
	int mag = 8, int max = 8, float rate = 100.0f)
{
	auto ammo = std::make_unique<engine::combat::bullet_ammunition>();
	ammo_out = ammo.get();
	return engine::combat::shotgun(std::move(ammo), mag, max, rate);
}

int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// pistol_fire — fire once: ammo drops 8→7, 1 bullet spawned
		if (cmd == "pistol_fire") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// pistol_empty — fire full magazine (with cooldown bypass), can_fire() = false
		} else if (cmd == "pistol_empty") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(p.can_fire() ? 1 : 0);

		// pistol_overshoot — fire more than magazine, check spawned count capped at 8
		} else if (cmd == "pistol_overshoot") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 12; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(static_cast<int>(a->spawned.size()));

		// pistol_reload — fire empty, reload: can_fire() = true, ammo_count = 8
		} else if (cmd == "pistol_reload") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			p.update(1.0f);
			p.reload();
			result(p.can_fire() ? 1 : 0);
			result(p.ammo_count);

		// pistol_cooldown_ready — fire, advance 0.6s (> 0.5s for rate=2.0), can fire
		} else if (cmd == "pistol_cooldown_ready") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.6f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_cooldown_block — fire, advance 0.3s (< 0.5s for rate=2.0), blocked
		} else if (cmd == "pistol_cooldown_block") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.3f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_no_fire_blocked — second fire during cooldown spawns nothing extra
		} else if (cmd == "pistol_no_fire_blocked") {
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, 2.0f);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(0.3f);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(static_cast<int>(a->spawned.size()));

		// pistol_bullet_pos <px> <py> <angle> — bullet spawned at correct position
		} else if (cmd == "pistol_bullet_pos") {
			float px, py, ang;
			std::cin >> px >> py >> ang;
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			p.fire({px, py}, ang);
			result(a->spawned[0].pos.x);
			result(a->spawned[0].pos.y);
			result(a->spawned[0].angle);

		// =====================================================================
		// PARAMETRIC
		// =====================================================================

		// fire_count <shots> — fire <shots> times (mag=8, rate=100), report ammo and spawned
		} else if (cmd == "fire_count") {
			int shots;
			std::cin >> shots;
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a);
			for (int i = 0; i < shots; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			result(p.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// cooldown_check <rate> <dt> — fire once, advance dt, report can_fire
		} else if (cmd == "cooldown_check") {
			float rate, dt;
			std::cin >> rate >> dt;
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a, 8, 8, rate);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.update(dt);
			result(p.can_fire() ? "YES" : "NO");

		// reload_check <mag_size> — fire empty, reload, report ammo_count
		} else if (cmd == "reload_check") {
			int mag;
			std::cin >> mag;
			engine::combat::bullet_ammunition* a;
			auto p = make_pistol(a, mag, mag);
			for (int i = 0; i < mag; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.update(1.0f); }
			p.reload();
			result(p.ammo_count);

		// =====================================================================
		// SMG
		// =====================================================================

		// smg_fire — fire once: ammo 30→29, 1 bullet spawned
		} else if (cmd == "smg_fire") {
			engine::combat::bullet_ammunition* a;
			auto s = make_smg(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// =====================================================================
		// RIFLE
		// =====================================================================

		// rifle_fire — fire once: ammo 20→19, 1 bullet spawned
		} else if (cmd == "rifle_fire") {
			engine::combat::bullet_ammunition* a;
			auto r = make_rifle(a);
			r.fire({0.0f, 0.0f}, 0.0f);
			result(r.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// =====================================================================
		// SNIPER RIFLE
		// =====================================================================

		// sniper_fire — fire once: ammo 5→4, 1 bullet spawned
		} else if (cmd == "sniper_fire") {
			engine::combat::bullet_ammunition* a;
			auto s = make_sniper(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// =====================================================================
		// PLASMA GUN
		// =====================================================================

		// plasma_fire — fire once: ammo 10→9, 1 plasma spawned
		} else if (cmd == "plasma_fire") {
			engine::combat::plasma_ammunition* a;
			auto p = make_plasma(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// =====================================================================
		// SHOTGUN
		// =====================================================================

		// shotgun_fire — fire once: ammo 8→7, pellet_count pellets spawned
		} else if (cmd == "shotgun_fire") {
			engine::combat::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s.ammo_count);
			result(static_cast<int>(a->spawned.size()));

		// shotgun_spread — fire at angle 0, report min and max pellet angles
		} else if (cmd == "shotgun_spread") {
			engine::combat::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float mn = a->spawned[0].angle, mx = a->spawned[0].angle;
			for (auto& r : a->spawned) {
				mn = std::min(mn, r.angle);
				mx = std::max(mx, r.angle);
			}
			result(mn);
			result(mx);

		// shotgun_pellet_count <shots> — fire <shots> times, report total pellets
		} else if (cmd == "shotgun_pellet_count") {
			int shots;
			std::cin >> shots;
			engine::combat::bullet_ammunition* a;
			auto s = make_shotgun(a);
			for (int i = 0; i < shots; ++i) { s.fire({0.0f, 0.0f}, 0.0f); s.update(1.0f); }
			result(static_cast<int>(a->spawned.size()));

		// =====================================================================
		// KATANA
		// =====================================================================

		// katana_swing — swing once: swing_count=1, immediately blocked
		} else if (cmd == "katana_swing") {
			engine::combat::katana k;
			k.fire({0.0f, 0.0f}, 0.0f);
			result(k.swing_count);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown — swing, advance 0.5s (< 1/1.5≈0.667s), blocked
		} else if (cmd == "katana_cooldown") {
			engine::combat::katana k(1.5f);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.update(0.5f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown_ready — swing, advance 0.7s (> 0.667s), ready
		} else if (cmd == "katana_cooldown_ready") {
			engine::combat::katana k(1.5f);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.update(0.7f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_swings <n> — swing n times (cooldown bypass), report swing_count
		} else if (cmd == "katana_swings") {
			int n;
			std::cin >> n;
			engine::combat::katana k;
			for (int i = 0; i < n; ++i) { k.fire({0.0f, 0.0f}, 0.0f); k.update(1.0f); }
			result(k.swing_count);

		// =====================================================================
		// DAMAGE
		// =====================================================================

		// damage_check <weapon> — fire once, report damage stored in spawned record
		// Weapons: pistol(25) smg(10) rifle(35) sniper(100) plasma(75) shotgun(15) katana(50)
		} else if (cmd == "damage_check") {
			std::string w;
			std::cin >> w;
			if (w == "pistol") {
				engine::combat::bullet_ammunition* a;
				auto p = make_pistol(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage);
			} else if (w == "smg") {
				engine::combat::bullet_ammunition* a;
				auto s = make_smg(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage);
			} else if (w == "rifle") {
				engine::combat::bullet_ammunition* a;
				auto r = make_rifle(a);
				r.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage);
			} else if (w == "sniper") {
				engine::combat::bullet_ammunition* a;
				auto s = make_sniper(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage);
			} else if (w == "plasma") {
				engine::combat::plasma_ammunition* a;
				auto p = make_plasma(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage);
			} else if (w == "shotgun") {
				engine::combat::bullet_ammunition* a;
				auto s = make_shotgun(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result(a->spawned[0].damage); // damage per pellet
			} else if (w == "katana") {
				engine::combat::katana k;
				result(k.damage);
			} else {
				result("UNKNOWN");
			}

		// shotgun_total_damage — fire once, sum damage of all pellets
		} else if (cmd == "shotgun_total_damage") {
			engine::combat::bullet_ammunition* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float total = 0.0f;
			for (auto& r : a->spawned) total += r.damage;
			result(total);

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
