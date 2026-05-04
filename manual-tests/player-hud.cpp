#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "entities/entities.h"
#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/katana.h"
#include "combat/weapons/bullet.h"
#include "combat/weapons/plasma-ammo.h"
#include "combat/burning.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"

template<typename M>
struct inspect : public M {
	using M::M;
	float hp()          const { return this->health.current_hp; }
	float max_hp()      const { return this->health.max_hp; }
	float armor()       const { return this->health.armor; }
	bool  dead()        const { return this->health.is_dead(); }
	int   effect_count() const { return static_cast<int>(this->health.active_effects.size()); }
};

struct weapon_slot {
	std::unique_ptr<engine::combat::weapon> owned;
	std::string name;
	int   reserve_mags;
	float reload_timer;
	float reload_duration;
	bool  in_loadout;
	bool  is_melee;
	bool  is_auto;
};

enum class cmd_mode { none, damage, heal, armor };

static std::string hbar(float cur, float max, int w = 20) {
	int n = (max > 0.0f) ? std::min(w, static_cast<int>(cur / max * w + 0.5f)) : 0;
	std::string s = "[";
	for (int i = 0; i < w; ++i) s += (i < n ? '#' : '-');
	return s + "]";
}

static weapon_slot* find_slot(std::vector<weapon_slot>& pool,
                               engine::combat::weapon* cur)
{
	if (!cur) return nullptr;
	for (auto& s : pool)
		if (s.owned.get() == cur) return &s;
	return nullptr;
}

int main() {
	std::cout << "\033[2J\033[H\033[?25l" << std::flush;

	// ---- weapon pool ----
	std::vector<weapon_slot> pool;

	auto add_bullet = [&](auto wptr, const std::string& name,
	                       int reserve, float reload_dur, bool is_auto = false) {
		pool.push_back({ std::move(wptr), name,
		                 reserve, 0.0f, reload_dur, false, false, is_auto });
	};

	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_bullet(std::make_unique<engine::combat::pistol>(std::move(a)),      "Pistol",     5, 1.5f);       }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_bullet(std::make_unique<engine::combat::smg>(std::move(a)),         "SMG",        4, 2.5f, true); }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_bullet(std::make_unique<engine::combat::rifle>(std::move(a)),       "Rifle",      4, 2.0f, true); }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_bullet(std::make_unique<engine::combat::shotgun>(std::move(a)),     "Shotgun",    4, 3.0f);       }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_bullet(std::make_unique<engine::combat::sniper_rifle>(std::move(a)),"Sniper",     3, 3.5f);       }
	{ auto a = std::make_unique<engine::combat::plasma_ammunition>();
	  add_bullet(std::make_unique<engine::combat::plasma_gun>(std::move(a)),  "Plasma Gun", 3, 2.0f);       }
	pool.push_back({ std::make_unique<engine::combat::katana>(),
	                 "Katana", 0, 0.0f, 0.0f, false, true, false });

	pool[0].in_loadout = true;  // start with pistol

	// ---- player ----
	inspect<engine::entities::player> p(100.0f, 50.0f, 2.0f, 1.0f);

	auto rebuild_loadout = [&]() {
		engine::combat::weapon* preserve = p.current_weapon;
		p.weapons.clear();
		for (auto& s : pool)
			if (s.in_loadout) p.weapons.push_back(s.owned.get());
		p.current_weapon       = nullptr;
		p.current_weapon_index = 0;
		if (!p.weapons.empty()) {
			for (int i = 0; i < (int)p.weapons.size(); ++i) {
				if (p.weapons[i] == preserve) { p.switch_weapons(i); return; }
			}
			p.switch_weapons(0);
		}
	};
	rebuild_loadout();

	// ---- input backend ----
	auto backend = std::make_unique<input::evdev::backend>();
	if (backend->is_bad()) {
		std::cout << "\033[?25h";
		std::cerr << "error: failed to initialize evdev backend\n"
		          << "(check permissions for /dev/input/event*)\n";
		return 1;
	}

	// Command-mode state
	cmd_mode mode = cmd_mode::none;
	std::string num_buf;

	// Edge-trigger state
	bool prev_lmb   = false;
	bool prev_r     = false;
	bool prev_plus  = false;
	bool prev_minus = false;
	bool prev_d     = false;
	bool prev_h     = false;
	bool prev_a     = false;
	bool prev_b     = false;
	bool prev_v     = false;
	bool prev_enter = false;
	bool prev_bs    = false;
	bool prev_num[10] = {};   // index 0=n0 .. 9=n9

	const input::key numkeys[10] = {
		input::key::n0, input::key::n1, input::key::n2, input::key::n3, input::key::n4,
		input::key::n5, input::key::n6, input::key::n7, input::key::n8, input::key::n9
	};

	auto edge = [&](input::key k, bool& prev) -> bool {
		bool cur   = backend->is_key_down(k);
		bool fired = cur && !prev;
		prev       = cur;
		return fired;
	};

	auto last = std::chrono::steady_clock::now();

	while (true) {
		auto now = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(now - last).count();
		last = now;

		backend->update();
		if (backend->is_bad()) break;

		bool game_over = p.dead();

		// ESC: cancel command input, or exit
		if (backend->is_key_down(input::key::esc)) {
			if (mode != cmd_mode::none) {
				mode = cmd_mode::none;
				num_buf.clear();
			} else {
				break;
			}
		}

		weapon_slot* cur_slot = find_slot(pool, p.current_weapon);

		if (game_over) {
			if (edge(input::key::v, prev_v))
				p.heal(p.max_hp());
		} else {
			// Weapon switch 1-9 (disabled while entering a number)
			if (mode == cmd_mode::none) {
				for (int i = 1; i <= 9; ++i) {
					bool down = backend->is_key_down(numkeys[i]);
					if (down && !prev_num[i]) p.switch_weapons(i - 1);
					prev_num[i] = down;
				}
				cur_slot = find_slot(pool, p.current_weapon);
			}

			// Shoot — auto weapons fire while LMB held, semi on press edge
			{
				auto mouse = backend->get_mouse_state();
				bool reloading = cur_slot && cur_slot->reload_timer > 0.0f;
				bool triggered = cur_slot && cur_slot->is_auto
				               ? mouse.left
				               : (mouse.left && !prev_lmb);
				if (!reloading && triggered) p.shoot();
				prev_lmb = mouse.left;
			}

			// Reload (R, edge trigger, costs one reserve mag)
			bool r_down = backend->is_key_down(input::key::r);
			if (r_down && !prev_r && cur_slot
			    && !cur_slot->is_melee
			    && cur_slot->reserve_mags > 0
			    && cur_slot->reload_timer <= 0.0f)
			{
				--cur_slot->reserve_mags;
				cur_slot->reload_timer = cur_slot->reload_duration;
			}
			prev_r = r_down;

			// Add next available weapon to loadout
			bool plus_down = backend->is_key_down(input::key::equals);
			if (plus_down && !prev_plus)
				for (auto& s : pool)
					if (!s.in_loadout) { s.in_loadout = true; rebuild_loadout(); break; }
			prev_plus = plus_down;

			// Remove current weapon from loadout
			bool minus_down = backend->is_key_down(input::key::hyphen);
			if (minus_down && !prev_minus && p.current_weapon)
				for (auto& s : pool)
					if (s.in_loadout && s.owned.get() == p.current_weapon) {
						s.in_loadout = false; rebuild_loadout(); break;
					}
			prev_minus = minus_down;

			// D / H / A: open number-input command; B: instant burning
			if (mode == cmd_mode::none) {
				if (edge(input::key::d, prev_d)) { mode = cmd_mode::damage; num_buf.clear(); }
				if (edge(input::key::h, prev_h)) { mode = cmd_mode::heal;   num_buf.clear(); }
				if (edge(input::key::a, prev_a)) { mode = cmd_mode::armor;  num_buf.clear(); }
				if (edge(input::key::b, prev_b))
					p.add_effect(std::make_unique<engine::combat::burning>(5.0f, 5));
			}

			// Number keys / Backspace / Enter while in command mode
			if (mode != cmd_mode::none) {
				for (int i = 0; i < 10; ++i) {
					bool down = backend->is_key_down(numkeys[i]);
					if (down && !prev_num[i]) num_buf += static_cast<char>('0' + i);
					prev_num[i] = down;
				}
				if (edge(input::key::backspace, prev_bs) && !num_buf.empty())
					num_buf.pop_back();
				if (edge(input::key::enter, prev_enter)) {
					if (!num_buf.empty()) {
						float val = static_cast<float>(std::stoi(num_buf));
						if      (mode == cmd_mode::damage) p.take_damage(val);
						else if (mode == cmd_mode::heal)   p.heal(val);
						else if (mode == cmd_mode::armor)  p.add_shield(val);
					}
					mode = cmd_mode::none;
					num_buf.clear();
				}
			}

			// Tick reload timers
			for (auto& s : pool) {
				if (s.reload_timer > 0.0f) {
					s.reload_timer -= dt;
					if (s.reload_timer <= 0.0f) {
						s.reload_timer = 0.0f;
						s.owned->reload();
					}
				}
			}
		}

		p.update(dt);  // ticks status effects regardless of alive state

		// ---- render HUD ----
		cur_slot = find_slot(pool, p.current_weapon);
		std::cout << "\033[H";

		std::cout << (game_over ? "=== GAME  OVER  ===" : "=== PLAYER STATUS ===") << "\033[K\n";
		std::cout << "\033[K\n";

		std::cout << "HP:    " << std::fixed << std::setprecision(1)
		          << p.hp() << " / " << p.max_hp()
		          << "  " << hbar(p.hp(), p.max_hp()) << "\033[K\n";
		std::cout << "Armor: " << std::fixed << std::setprecision(1)
		          << p.armor() << "\033[K\n";
		if (p.effect_count() > 0)
			std::cout << "FX:    BURNING\033[K\n";
		else
			std::cout << "\033[K\n";

		std::cout << "\033[K\n";

		if (p.current_weapon && cur_slot) {
			std::cout << "Weapon: " << cur_slot->name
			          << (cur_slot->is_auto ? " [AUTO]" : "")
			          << "  (slot " << (p.current_weapon_index + 1)
			          << " / " << p.weapons.size() << ")\033[K\n";

			// Status line (reload / cooldown / ready)
			std::string status;
			if (cur_slot->reload_timer > 0.0f) {
				std::ostringstream oss;
				oss << "RELOADING... " << std::fixed << std::setprecision(1)
				    << cur_slot->reload_timer << "s";
				status = oss.str();
			} else if (!cur_slot->is_melee && p.current_weapon->ammo_count == 0
			           && cur_slot->reserve_mags > 0) {
				status = "[press R to reload]";
			} else if (!cur_slot->is_auto) {
				float lst    = p.current_weapon->last_shot_time;
				float max_cd = 1.0f / p.current_weapon->fire_rate;
				if (lst > 0.005f) {
					std::ostringstream oss;
					oss << (cur_slot->is_melee ? "Next swing: " : "Next shot:  ")
					    << std::fixed << std::setprecision(2) << lst << "s  "
					    << hbar(1.0f - lst / max_cd, 1.0f, 12);
					status = oss.str();
				} else {
					status = "Ready";
				}
			}

			if (cur_slot->is_melee) {
				std::cout << "Ammo:   ---  (melee)\033[K\n";
				std::cout << "Mags:   ---\033[K\n";
			} else {
				std::cout << "Ammo:   "
				          << p.current_weapon->ammo_count
				          << " / " << p.current_weapon->max_ammo
				          << "  " << hbar(p.current_weapon->ammo_count,
				                          p.current_weapon->max_ammo, 15)
				          << "\033[K\n";
				std::cout << "Mags:   " << cur_slot->reserve_mags << " remaining\033[K\n";
			}
			std::cout << "        " << status << "\033[K\n";
		} else {
			std::cout << "Weapon: (none)\033[K\n";
			std::cout << "Ammo:   ---\033[K\n";
			std::cout << "Mags:   ---\033[K\n";
			std::cout << "        \033[K\n";
		}

		std::cout << "\033[K\nLoadout:";
		for (int i = 0; i < (int)p.weapons.size(); ++i) {
			bool active = (p.current_weapon && i == p.current_weapon_index);
			std::string n;
			for (auto& s : pool)
				if (s.in_loadout && s.owned.get() == p.weapons[i]) { n = s.name; break; }
			std::cout << "  " << (i + 1) << ":"
			          << (active ? "[" : "") << n << (active ? "]" : "");
		}
		if (p.weapons.empty()) std::cout << "  (empty)";
		std::cout << "\033[K\n";

		std::cout << "\033[K\n";

		if (game_over) {
			std::cout << "        [V] Revive     [ESC] Exit\033[K\n";
			std::cout << "\033[K\n";
			std::cout << "\033[K\n";
			std::cout << "\033[K\n";
		} else if (mode != cmd_mode::none) {
			const char* prompt =
				(mode == cmd_mode::damage) ? "Damage amount" :
				(mode == cmd_mode::heal)   ? "Heal amount"   : "Armor amount";
			std::cout << "> " << prompt << ": " << num_buf << "_\033[K\n";
			std::cout << "  [0-9] type   [Backspace] delete   [Enter] apply   [ESC] cancel\033[K\n";
			std::cout << "\033[K\n";
			std::cout << "\033[K\n";
		} else {
			std::cout << "--- CONTROLS ---\033[K\n";
			std::cout << "1-9: switch   LMB: shoot   R: reload\033[K\n";
			std::cout << "=: add weapon   -: remove   ESC: exit\033[K\n";
			std::cout << "D: damage   H: heal   A: armor   B: burning (5s)\033[K\n";
		}

		std::cout << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	std::cout << "\033[?25h\033[2J\033[H" << std::flush;
	return 0;
}
