#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "entities/player.h"
#include "combat/weapons/pistol.h"
#include "combat/weapons/smg.h"
#include "combat/weapons/rifle.h"
#include "combat/weapons/shotgun.h"
#include "combat/weapons/sniper-rifle.h"
#include "combat/weapons/plasma-gun.h"
#include "combat/weapons/katana.h"
#include "combat/burning.h"

#include "geometry/map-data.h"
#include "engine/world.h"
#include "input/input-backend.h"
<<<<<<< HEAD
#include "input/evdev/backend.h"
=======
#include "input/evdev-backend.h"
>>>>>>> 7772340b1660540fe4cd5d85418048d9d66933dc
#include "audio/audio-mixer.h"
#include "audio/alsa/backend.h"
#include "util/resource-loader.h"
#include "assets/asset-manager.h"

template<typename M>
struct inspect : public M {
	using M::M;
	using M::weapons;
	using M::current_weapon_index;
	float hp()          const { return this->health("current_hp"_f); }
	float max_hp()      const { return this->health("max_hp"_f); }
	float armor()       const { return this->health("armor"_f); }
	float max_armor()   const { return this->health("max_armor"_f); }
	bool  dead()        const { return this->health.is_dead(); }
	int   effect_count() const { return static_cast<int>(this->health("active_effects"_f).size()); }
};

// Expose protected weapon fields via pointer-to-member.
struct wp : combat::weapons::weapon {
	using weapon::ammo_count;
	using weapon::max_ammo;
	using weapon::reserve_mags;
	using weapon::last_shot_time;
	using weapon::fire_rate;
};

struct weapon_slot {
	combat::weapons::weapon* ref;   // non-owning, for identification
	std::string name;
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
                               combat::weapons::weapon* cur)
{
	if (!cur) return nullptr;
	for (auto& s : pool)
		if (s.ref == cur) return &s;
	return nullptr;
}

int main() {
	std::cout << "\033[2J\033[H\033[?25l" << std::flush;

	// ---- resources & audio ----
	util::resource_loader rl;
	auto tex_mgr = assets::asset_manager::load(rl);
	auto a_back = std::make_unique<audio::alsa::backend>();
	audio::audio_mixer mix(*a_back, audio::audio_format{48000, 2, 16});

	// ---- player ----
	geometry::map_data md;
	engine::world w;
	inspect<entities::player> p({0.0f, 0.0f}, 0.0f, 0, 1.0f, 100.0f, 50.0f, 2.0f, 1.0f);

	// ---- weapon pool (metadata) ----
	std::vector<weapon_slot> pool;

	auto add_weapon = [&](auto wptr, const std::string& name,
	                       float reload_dur, bool melee = false, bool is_auto = false) {
		p.weapons.push_back(std::move(wptr));
		pool.push_back({ &*p.weapons.back(), name,
		                 0.0f, reload_dur, false, melee, is_auto });
	};

	add_weapon(std::make_unique<combat::weapons::pistol>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(1), &tex_mgr.audio_clip_by_id(2)),       "Pistol",     1.5f);
	add_weapon(std::make_unique<combat::weapons::smg>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(3), &tex_mgr.audio_clip_by_id(4)),       "SMG",        2.5f, false, true);
	add_weapon(std::make_unique<combat::weapons::rifle>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(5), &tex_mgr.audio_clip_by_id(6)),       "Rifle",      2.0f, false, true);
	add_weapon(std::make_unique<combat::weapons::shotgun>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(7), &tex_mgr.audio_clip_by_id(8)),       "Shotgun",    3.0f);
	add_weapon(std::make_unique<combat::weapons::sniper_rifle>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(9), &tex_mgr.audio_clip_by_id(10)),      "Sniper",     3.5f);
	add_weapon(std::make_unique<combat::weapons::plasma_gun>(md, w, &mix,
	               &tex_mgr.audio_clip_by_id(11), &tex_mgr.audio_clip_by_id(12)),     "Plasma Gun", 2.0f);
	add_weapon(std::make_unique<combat::weapons::katana>(&mix,
	               &tex_mgr.audio_clip_by_id(13)),                                     "Katana",     0.0f, true);

	pool[0].in_loadout = true;  // start with pistol

	// Helper: get raw pointer to current weapon
	auto cur_wp = [&]() -> combat::weapons::weapon* {
		if (p.current_weapon_index < 0 || p.current_weapon_index >= (int)p.weapons.size())
			return nullptr;
		return &*p.weapons[p.current_weapon_index];
	};

	// Helper: get indices (into p.weapons) of in-loadout weapons, in pool order
	auto loadout_indices = [&]() -> std::vector<int> {
		std::vector<int> result;
		for (auto& s : pool) {
			if (!s.in_loadout) continue;
			for (int i = 0; i < (int)p.weapons.size(); ++i)
				if (&*p.weapons[i] == s.ref) { result.push_back(i); break; }
		}
		return result;
	};

	// Select the right weapon after loadout change
	auto rebuild_loadout = [&]() {
		auto li = loadout_indices();
		if (li.empty()) { p.current_weapon_index = -1; return; }
		for (int idx : li)
			if (idx == p.current_weapon_index) return;
		p.switch_weapons(li[0]);
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
	bool prev_num[10] = {};

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

		weapon_slot* cur_slot = find_slot(pool, cur_wp());

		if (game_over) {
			if (edge(input::key::v, prev_v))
				p.heal(p.max_hp());
		} else {
			// Weapon switch 1-9 (disabled while entering a number)
			if (mode == cmd_mode::none) {
				for (int i = 1; i <= 9; ++i) {
					bool down = backend->is_key_down(numkeys[i]);
					if (down && !prev_num[i]) {
						auto li = loadout_indices();
						if (i - 1 < (int)li.size()) p.switch_weapons(li[i - 1]);
					}
					prev_num[i] = down;
				}
				cur_slot = find_slot(pool, cur_wp());
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
			    && cur_slot->ref->*(&wp::reserve_mags) > 0
			    && cur_slot->reload_timer <= 0.0f)
			{
				cur_slot->ref->reload();
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
			if (minus_down && !prev_minus && cur_wp())
				for (auto& s : pool)
					if (s.in_loadout && s.ref == cur_wp()) {
						s.in_loadout = false; rebuild_loadout(); break;
					}
			prev_minus = minus_down;

			// D / H / A: open number-input command; B: instant burning
			if (mode == cmd_mode::none) {
				if (edge(input::key::d, prev_d)) { mode = cmd_mode::damage; num_buf.clear(); }
				if (edge(input::key::h, prev_h)) { mode = cmd_mode::heal;   num_buf.clear(); }
				if (edge(input::key::a, prev_a)) { mode = cmd_mode::armor;  num_buf.clear(); }
				if (edge(input::key::b, prev_b))
					p.add_effect(std::make_unique<combat::burning>(5.0f, 5));
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
					if (s.reload_timer <= 0.0f)
						s.reload_timer = 0.0f;
				}
			}
		}

		p.update(dt);  // ticks status effects regardless of alive state
		mix.step(static_cast<unsigned long>(48000 * dt));

		// ---- render HUD ----
		auto* cw = cur_wp();
		cur_slot = find_slot(pool, cw);
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

		if (cw && cur_slot) {
			auto li = loadout_indices();
			int slot_pos = 0;
			for (int i = 0; i < (int)li.size(); ++i)
				if (li[i] == p.current_weapon_index) { slot_pos = i; break; }

			std::cout << "Weapon: " << cur_slot->name
			          << (cur_slot->is_auto ? " [AUTO]" : "")
			          << "  (slot " << (slot_pos + 1)
			          << " / " << li.size() << ")\033[K\n";

			// Status line (reload / cooldown / ready)
			std::string status;
			if (cur_slot->reload_timer > 0.0f) {
				std::ostringstream oss;
				oss << "RELOADING... " << std::fixed << std::setprecision(1)
				    << cur_slot->reload_timer << "s";
				status = oss.str();
			} else if (!cur_slot->is_melee && cw->*(&wp::ammo_count) == 0
			           && cw->*(&wp::reserve_mags) > 0) {
				status = "[press R to reload]";
			} else if (!cur_slot->is_auto) {
				float lst    = cw->*(&wp::last_shot_time);
				float max_cd = 1.0f / cw->*(&wp::fire_rate);
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
				          << cw->*(&wp::ammo_count)
				          << " / " << cw->*(&wp::max_ammo)
				          << "  " << hbar(cw->*(&wp::ammo_count),
				                          cw->*(&wp::max_ammo), 15)
				          << "\033[K\n";
				std::cout << "Mags:   " << cw->*(&wp::reserve_mags) << " remaining\033[K\n";
			}
			std::cout << "        " << status << "\033[K\n";
		} else {
			std::cout << "Weapon: (none)\033[K\n";
			std::cout << "Ammo:   ---\033[K\n";
			std::cout << "Mags:   ---\033[K\n";
			std::cout << "        \033[K\n";
		}

		std::cout << "\033[K\nLoadout:";
		int slot_n = 0;
		for (auto& s : pool) {
			if (!s.in_loadout) continue;
			bool active = (s.ref == cw);
			std::cout << "  " << (slot_n + 1) << ":"
			          << (active ? "[" : "") << s.name << (active ? "]" : "");
			++slot_n;
		}
		if (slot_n == 0) std::cout << "  (empty)";
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
