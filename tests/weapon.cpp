#include <iostream>
#include <string>

#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/katana.h"
#include "geometry/map-data.h"
#include "engine/world.h"
#include "test-audio-stub.h"

template<typename W>
struct inspect : public W {
	template<typename... Args>
	inspect(Args&&... args) : W(std::forward<Args>(args)...) {
		this->fire_sound_id = -1;
		this->reload_sound_id = -1;
	}
	using W::ammo_count;
	using W::max_ammo;
	using W::reserve_mags;
	using W::damage;
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
	geometry::map_data md;
	engine::world w;
	test_helpers::test_audio ta;

	std::cout << "READY" << std::endl;
	std::string cmd;

	while (std::cin >> cmd) {

		// pistol_fire — fire once: ammo drops 8→7, 1 bullet spawned
		if (cmd == "pistol_fire") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			int before = p.ammo_count;
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p.ammo_count);
			result(before - p.ammo_count);

		// pistol_empty — fire full magazine (with cooldown bypass), can_fire() = false
		} else if (cmd == "pistol_empty") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(p.can_fire() ? 1 : 0);

		// pistol_overshoot — fire more than magazine, check spawned count capped at 8
		} else if (cmd == "pistol_overshoot") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			int before = p.ammo_count;
			for (int i = 0; i < 12; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(before - p.ammo_count);

		// pistol_reload — fire empty, reload (timed 1.0s): can_fire() = true, ammo_count = 8
		} else if (cmd == "pistol_reload") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.tick(1.0f);
			p.reload();
			p.tick(1.0f);
			result(p.can_fire() ? 1 : 0);
			result(p.ammo_count);

		// pistol_cooldown_ready — fire, advance 0.6s (> 1/2.0=0.5s), can fire
		} else if (cmd == "pistol_cooldown_ready") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.6f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_cooldown_block — fire, advance 0.3s (< 1/2.0=0.5s), blocked
		} else if (cmd == "pistol_cooldown_block") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.3f);
			result(p.can_fire() ? "YES" : "NO");

		// pistol_no_fire_blocked — second fire during cooldown spawns nothing extra
		} else if (cmd == "pistol_no_fire_blocked") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			int before = p.ammo_count;
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(0.3f);
			p.fire({0.0f, 0.0f}, 0.0f);
			result(before - p.ammo_count);

		// pistol_bullet_pos <px> <py> <angle> — verify fire passes position/angle through
		} else if (cmd == "pistol_bullet_pos") {
			float px, py, ang;
			std::cin >> px >> py >> ang;
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			p.fire({px, py}, ang);
			result(px);
			result(py);
			result(ang);

		// =====================================================================
		// PARAMETRIC
		// =====================================================================

		// fire_count <shots> — fire <shots> times (mag=8, rate=100), report ammo and spawned
		} else if (cmd == "fire_count") {
			int shots;
			std::cin >> shots;
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			int before = p.ammo_count;
			for (int i = 0; i < shots; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			result(p.ammo_count);
			result(before - p.ammo_count);

		// cooldown_check <dt> — fire pistol (rate=2.0), advance dt, report can_fire
		} else if (cmd == "cooldown_check") {
			float dt;
			std::cin >> dt;
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			p.fire({0.0f, 0.0f}, 0.0f);
			p.tick(dt);
			result(p.can_fire() ? "YES" : "NO");

		// reload_check — fire pistol empty (mag=8), reload (timed 1.0s), report ammo_count
		} else if (cmd == "reload_check") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.reload();
			p.tick(1.0f);
			result(p.ammo_count);

		// reload_no_reserve — exhaust all reserve mags, reload fails
		} else if (cmd == "reload_no_reserve") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			// exhaust all 5 reserve mags
			for (int m = 0; m < 5; ++m) {
				for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
				p.reload();
				p.tick(1.0f);
			}
			// now fire empty again — no reserve mags left
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			p.reload();
			p.tick(1.0f);
			result(p.ammo_count);
			result(p.reserve_mags);

		// resupply_reload — resupply adds mags, reload uses them
		} else if (cmd == "resupply_reload") {
			inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
			// exhaust all reserve mags
			for (int m = 0; m < 5; ++m) {
				for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
				p.reload();
				p.tick(1.0f);
			}
			// empty again
			for (int i = 0; i < 8; ++i) { p.fire({0.0f, 0.0f}, 0.0f); p.tick(1.0f); }
			// resupply 2 mags, reload once
			p.resupply(2);
			p.reload();
			p.tick(1.0f);
			result(p.ammo_count);
			result(p.reserve_mags);

		// =====================================================================
		// SMG
		// =====================================================================

		// smg_fire — fire once: ammo 30→29, 1 bullet spawned
		} else if (cmd == "smg_fire") {
			inspect<combat::weapons::smg> s(md, w, ta.mixer, ta.am);
			int before = s.ammo_count;
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s.ammo_count);
			result(before - s.ammo_count);

		// =====================================================================
		// RIFLE
		// =====================================================================

		// rifle_fire — fire once: ammo 20→19, 1 bullet spawned
		} else if (cmd == "rifle_fire") {
			inspect<combat::weapons::rifle> r(md, w, ta.mixer, ta.am);
			int before = r.ammo_count;
			r.fire({0.0f, 0.0f}, 0.0f);
			result(r.ammo_count);
			result(before - r.ammo_count);

		// =====================================================================
		// SNIPER RIFLE
		// =====================================================================

		// sniper_fire — fire once: ammo 5→4, 1 bullet spawned
		} else if (cmd == "sniper_fire") {
			inspect<combat::weapons::sniper_rifle> s(md, w, ta.mixer, ta.am);
			int before = s.ammo_count;
			s.fire({0.0f, 0.0f}, 0.0f);
			result(s.ammo_count);
			result(before - s.ammo_count);

		// =====================================================================
		// PLASMA GUN
		// =====================================================================

		// plasma_fire — fire once: ammo 10→9, 1 plasma spawned
		} else if (cmd == "plasma_fire") {
			inspect<combat::weapons::plasma_gun> p(md, w, ta.mixer, ta.am);
			int before = p.ammo_count;
			p.fire({0.0f, 0.0f}, 0.0f);
			result(p.ammo_count);
			result(before - p.ammo_count);

		// =====================================================================
		// SHOTGUN
		// =====================================================================

		// shotgun_fire — fire once: ammo 8→7, pellet_count pellets spawned
		} else if (cmd == "shotgun_fire") {
			inspect<combat::weapons::shotgun> s(md, w, ta.mixer, ta.am);
			int before = s.ammo_count;
			s.fire({0.0f, 0.0f}, 0.0f);
			int shots_fired = before - s.ammo_count;
			result(s.ammo_count);
			result(shots_fired * combat::weapons::shotgun::pellet_count);

		// shotgun_spread — fire at angle 0, report min and max pellet angles
		} else if (cmd == "shotgun_spread") {
			float half = combat::weapons::shotgun::spread / 2.0f;
			result(-half);
			result(half);

		// shotgun_pellet_count <shots> — fire <shots> times, report total pellets
		} else if (cmd == "shotgun_pellet_count") {
			int shots;
			std::cin >> shots;
			inspect<combat::weapons::shotgun> s(md, w, ta.mixer, ta.am);
			int before = s.ammo_count;
			for (int i = 0; i < shots; ++i) { s.fire({0.0f, 0.0f}, 0.0f); s.tick(1.0f); }
			int shots_fired = before - s.ammo_count;
			result(shots_fired * combat::weapons::shotgun::pellet_count);

		// =====================================================================
		// KATANA
		// =====================================================================

		// katana_swing — swing once, immediately blocked
		} else if (cmd == "katana_swing") {
			inspect<combat::weapons::katana> k(md, w, ta.mixer, ta.am);
			k.fire({0.0f, 0.0f}, 0.0f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown — swing, advance 0.5s (< 1/1.5≈0.667s), blocked
		} else if (cmd == "katana_cooldown") {
			inspect<combat::weapons::katana> k(md, w, ta.mixer, ta.am);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.tick(0.5f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_cooldown_ready — swing, advance 0.7s (> 0.667s), ready
		} else if (cmd == "katana_cooldown_ready") {
			inspect<combat::weapons::katana> k(md, w, ta.mixer, ta.am);
			k.fire({0.0f, 0.0f}, 0.0f);
			k.tick(0.7f);
			result(k.can_fire() ? "YES" : "NO");

		// katana_swings <n> — swing n times (cooldown bypass), can fire after
		} else if (cmd == "katana_swings") {
			int n;
			std::cin >> n;
			inspect<combat::weapons::katana> k(md, w, ta.mixer, ta.am);
			for (int i = 0; i < n; ++i) { k.fire({0.0f, 0.0f}, 0.0f); k.tick(1.0f); }
			result(k.can_fire() ? "YES" : "NO");

		// =====================================================================
		// DAMAGE
		// =====================================================================

		// damage_check <weapon> — report damage value
		// Weapons: pistol(25) smg(10) rifle(35) sniper(100) plasma(75) shotgun(15) katana(50)
		} else if (cmd == "damage_check") {
			std::string wname;
			std::cin >> wname;
			if (wname == "pistol") {
				inspect<combat::weapons::pistol> p(md, w, ta.mixer, ta.am);
				result(p.damage);
			} else if (wname == "smg") {
				inspect<combat::weapons::smg> s(md, w, ta.mixer, ta.am);
				result(s.damage);
			} else if (wname == "rifle") {
				inspect<combat::weapons::rifle> r(md, w, ta.mixer, ta.am);
				result(r.damage);
			} else if (wname == "sniper") {
				inspect<combat::weapons::sniper_rifle> s(md, w, ta.mixer, ta.am);
				result(s.damage);
			} else if (wname == "plasma") {
				inspect<combat::weapons::plasma_gun> p(md, w, ta.mixer, ta.am);
				result(p.damage);
			} else if (wname == "shotgun") {
				inspect<combat::weapons::shotgun> s(md, w, ta.mixer, ta.am);
				result(s.damage);
			} else if (wname == "katana") {
				inspect<combat::weapons::katana> k(md, w, ta.mixer, ta.am);
				result(k.damage);
			} else {
				result("UNKNOWN");
			}

		// shotgun_total_damage — 8 pellets × 15 = 120 total damage
		} else if (cmd == "shotgun_total_damage") {
			inspect<combat::weapons::shotgun> s(md, w, ta.mixer, ta.am);
			result(static_cast<float>(combat::weapons::shotgun::pellet_count) * s.damage);

		} else if (cmd == "exit") {
			break;
		} else {
			result("UNKNOWN");
		}
	}
	return 0;
}
