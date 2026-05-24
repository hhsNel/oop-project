#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/katana.h"
#include "combat/weapons/firing-mode.h"
#include "util/componentized.h"

namespace combat { namespace weapons {
	class recorded_firing_mode : public firing_mode, public util::componentized<recorded_firing_mode> {
		friend class util::componentized<recorded_firing_mode>;
	public:
		struct shot_record { math::vec2 pos; float angle; float damage; };
	private:
		[[=util::component_field{}]] std::vector<shot_record> spawned;
	public:
		void spawn_bullet(math::vec2 pos, float angle, float damage) override {
			spawned.push_back({pos, angle, damage});
		}
	};
}}

static void result(float v) {
	std::cout << "RESULT " << static_cast<int>(v * 100 + 0.5f) / 100.0f << std::endl;
}
static void result(int v) {
	std::cout << "RESULT " << v << std::endl;
}
static void result(const std::string& s) {
	std::cout << "RESULT " << s << std::endl;
}

// Helpers — weapons use hardcoded defaults; update(1.0f) bypasses cooldown between fires.
static combat::weapons::pistol make_pistol(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::pistol(std::move(ammo));
}

static combat::weapons::smg make_smg(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::smg(std::move(ammo));
}

static combat::weapons::rifle make_rifle(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::rifle(std::move(ammo));
}

static combat::weapons::sniper_rifle make_sniper(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::sniper_rifle(std::move(ammo));
}

static combat::weapons::plasma_gun make_plasma(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::plasma_gun(std::move(ammo));
}

static combat::weapons::shotgun make_shotgun(
	combat::weapons::recorded_firing_mode*& ammo_out)
{
	auto ammo = std::make_unique<combat::weapons::recorded_firing_mode>();
	ammo_out = ammo.get();
	return combat::weapons::shotgun(std::move(ammo));
}

int main() {
	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// pistol_fire — fire once: ammo drops 8→7, 1 bullet spawned
		if (cmd == "pistol_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// pistol_empty — fire full magazine (with cooldown bypass), can_fire() = false
		} else if (cmd == "pistol_empty") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(p.can_fire() ? 1 : 0);

		// pistol_overshoot — fire more than magazine, check spawned count capped at 8
		} else if (cmd == "pistol_overshoot") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 12; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(static_cast<int>((*a)("spawned"_f).size()));

		// pistol_reload — fire empty, reload: can_fire() = true, ammo_count = 8
		} else if (cmd == "pistol_reload") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.tick(1.0f);
			p.reload();
			result(p.can_fire() ? 1 : 0);
			result(p("ammo_count"_f));

		// pistol_cooldown_ready — fire, advance 0.6s (> 1/2.0=0.5s), can fire
		} else if (cmd == "pistol_cooldown_ready") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.6f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_cooldown_block — fire, advance 0.3s (< 1/2.0=0.5s), blocked
		} else if (cmd == "pistol_cooldown_block") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.3f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_no_fire_blocked — second fire during cooldown spawns nothing extra
		} else if (cmd == "pistol_no_fire_blocked") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.3f);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(static_cast<int>((*a)("spawned"_f).size()));

		// pistol_bullet_pos <px> <py> <angle> — bullet spawned at correct position
		} else if (cmd == "pistol_bullet_pos") {
			float px, py, ang;
			std::cin >> px >> py >> ang;
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({px, py}, ang);
			result((*a)("spawned"_f)[0].pos("x"_f));
			result((*a)("spawned"_f)[0].pos("y"_f));
			result((*a)("spawned"_f)[0].angle);

		// =====================================================================
		// PARAMETRIC
		// =====================================================================

		// fire_count <shots> — fire <shots> times (mag=8, rate=100), report ammo and spawned
		} else if (cmd == "fire_count") {
			int shots;
			std::cin >> shots;
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			for (int i = 0; i < shots; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(p("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// cooldown_check <dt> — fire pistol (rate=2.0), advance dt, report can_fire
		} else if (cmd == "cooldown_check") {
			float dt;
			std::cin >> dt;
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(dt);
			result(p.can_fire() ? "YES" : "NO");

		// reload_check — fire pistol empty (mag=8), reload, report ammo_count
		} else if (cmd == "reload_check") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.reload();
			result(p("ammo_count"_f));

		// reload_no_reserve — exhaust all reserve mags, reload fails
		} else if (cmd == "reload_no_reserve") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			// exhaust all 5 reserve mags
			for (int m = 0; m < 5; ++m) {
				for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
				p.reload();
			}
			// now fire empty again — no reserve mags left
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.reload();
			result(p("ammo_count"_f));
			result(p("reserve_mags"_f));

		// resupply_reload — resupply adds mags, reload uses them
		} else if (cmd == "resupply_reload") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_pistol(a);
			// exhaust all reserve mags
			for (int m = 0; m < 5; ++m) {
				for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
				p.reload();
			}
			// empty again
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			// resupply 2 mags, reload once
			p.resupply(2);
			p.reload();
			result(p("ammo_count"_f));
			result(p("reserve_mags"_f));

		// =====================================================================
		// SMG
		// =====================================================================

		// smg_fire — fire once: ammo 30→29, 1 bullet spawned
		} else if (cmd == "smg_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto s = make_smg(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// =====================================================================
		// RIFLE
		// =====================================================================

		// rifle_fire — fire once: ammo 20→19, 1 bullet spawned
		} else if (cmd == "rifle_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto r = make_rifle(a);
			r.fire({0.0f, 0.0f}, 0.0f);
			result(r("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// =====================================================================
		// SNIPER RIFLE
		// =====================================================================

		// sniper_fire — fire once: ammo 5→4, 1 bullet spawned
		} else if (cmd == "sniper_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto s = make_sniper(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// =====================================================================
		// PLASMA GUN
		// =====================================================================

		// plasma_fire — fire once: ammo 10→9, 1 plasma spawned
		} else if (cmd == "plasma_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto p = make_plasma(a);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// =====================================================================
		// SHOTGUN
		// =====================================================================

		// shotgun_fire — fire once: ammo 8→7, pellet_count pellets spawned
		} else if (cmd == "shotgun_fire") {
			combat::weapons::recorded_firing_mode* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s("ammo_count"_f));
			result(static_cast<int>((*a)("spawned"_f).size()));

		// shotgun_spread — fire at angle 0, report min and max pellet angles
		} else if (cmd == "shotgun_spread") {
			combat::weapons::recorded_firing_mode* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float mn = (*a)("spawned"_f)[0].angle, mx = (*a)("spawned"_f)[0].angle;
			for (auto& r : (*a)("spawned"_f)) {
				mn = std::min(mn, r.angle);
				mx = std::max(mx, r.angle);
			}
			result(mn);
			result(mx);

		// shotgun_pellet_count <shots> — fire <shots> times, report total pellets
		} else if (cmd == "shotgun_pellet_count") {
			int shots;
			std::cin >> shots;
			combat::weapons::recorded_firing_mode* a;
			auto s = make_shotgun(a);
			for (int i = 0; i < shots; ++i) { s.fire({0.0f, 0.0f}, 0.0f); s.tick(1.0f); }
			result(static_cast<int>((*a)("spawned"_f).size()));

		// =====================================================================
		// KATANA
		// =====================================================================

		// katana_swing — swing once, immediately blocked
		} else if (cmd == "katana_swing") {
			combat::weapons::katana k;
			k.fire({0.0f, 0.0f}, 0.0f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown — swing, advance 0.5s (< 1/1.5≈0.667s), blocked
		} else if (cmd == "katana_cooldown") {
			combat::weapons::katana k;
			k.fire({0.0f, 0.0f}, 0.0f);
			k.tick(0.5f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown_ready — swing, advance 0.7s (> 0.667s), ready
		} else if (cmd == "katana_cooldown_ready") {
			combat::weapons::katana k;
			k.fire({0.0f, 0.0f}, 0.0f);
			k.tick(0.7f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_swings <n> — swing n times (cooldown bypass), can fire after
		} else if (cmd == "katana_swings") {
			int n;
			std::cin >> n;
			combat::weapons::katana k;
			for (int i = 0; i < n; ++i) { k.fire({0.0f, 0.0f}, 0.0f); k.tick(1.0f); }
			result(k.can_fire() ? "YES" : "NO");

		// =====================================================================
		// DAMAGE
		// =====================================================================

		// damage_check <weapon> — fire once, report damage stored in spawned record
		// Weapons: pistol(25) smg(10) rifle(35) sniper(100) plasma(75) shotgun(15) katana(50)
		} else if (cmd == "damage_check") {
			std::string w;
			std::cin >> w;
			if (w == "pistol") {
				combat::weapons::recorded_firing_mode* a;
				auto p = make_pistol(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage);
			} else if (w == "smg") {
				combat::weapons::recorded_firing_mode* a;
				auto s = make_smg(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage);
			} else if (w == "rifle") {
				combat::weapons::recorded_firing_mode* a;
				auto r = make_rifle(a);
				r.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage);
			} else if (w == "sniper") {
				combat::weapons::recorded_firing_mode* a;
				auto s = make_sniper(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage);
			} else if (w == "plasma") {
				combat::weapons::recorded_firing_mode* a;
				auto p = make_plasma(a);
				p.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage);
			} else if (w == "shotgun") {
				combat::weapons::recorded_firing_mode* a;
				auto s = make_shotgun(a);
				s.fire({0.0f, 0.0f}, 0.0f);
				result((*a)("spawned"_f)[0].damage); // damage per pellet
			} else if (w == "katana") {
				combat::weapons::katana k;
				result(k("damage"_f));
			} else {
				result("UNKNOWN");
			}

		// shotgun_total_damage — fire once, sum damage of all pellets
		} else if (cmd == "shotgun_total_damage") {
			combat::weapons::recorded_firing_mode* a;
			auto s = make_shotgun(a);
			s.fire({0.0f, 0.0f}, 0.0f);
			float total = 0.0f;
			for (auto& r : (*a)("spawned"_f)) total += r.damage;
			result(total);

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
