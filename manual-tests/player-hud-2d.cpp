#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include "util/resource-loader.h"
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
#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d-temp.h"
#include "graphics/texture-manager.h"

template<typename M>
struct inspect : public M {
	using M::M;
	float hp()          const { return this->health.current_hp; }
	float max_hp()      const { return this->health.max_hp; }
	float armor()       const { return this->health.armor; }
	float max_armor()   const { return this->health.max_armor; }
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

static weapon_slot* find_slot(std::vector<weapon_slot>& pool,
                               engine::combat::weapon* cur)
{
	if (!cur) return nullptr;
	for (auto& s : pool)
		if (s.owned.get() == cur) return &s;
	return nullptr;
}

static std::string fmt1(float v) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(1) << v;
	return oss.str();
}

int main() {
	// ---- rendering backend ----
	auto r_back = std::make_unique<rendering::drm_kms::backend>();
	if (r_back->is_bad()) {
		std::cerr << "error: failed to initialize DRM/KMS backend\n";
		return 1;
	}
	auto modes = r_back->get_modes();
	if (modes.empty()) { std::cerr << "error: no display modes\n"; return 1; }
	r_back->set_mode(std::move(modes[0]));
	if (r_back->is_bad()) { std::cerr << "error: set_mode failed\n"; return 1; }

	util::resource_loader rl;
	auto tex_mgr = graphics::texture_manager::load(rl);
	rendering::renderer_2d_temp r2d;
	r2d.set_target(r_back.get());
	r2d.set_texture_manager(&tex_mgr);
	r2d.set_font_texture(&tex_mgr.flat_tx_by_id(0));  // flat-font-atlas.btx

	const int SW = static_cast<int>(r_back->get_width());
	const int SH = static_cast<int>(r_back->get_height());

	// ---- layout ----
	const int CW  = 14;        // char width (normal)
	const int CH  = 22;        // char height (normal)
	const int LS  = CH + 4;    // line spacing
	const int SCW = 12;        // small char width — must be >= atlas cell width (12) to avoid column skipping
	const int SCH = 16;        // small char height
	const int PX  = 24;        // panel left
	const int PY  = 20;        // panel top
	const int LBL = 72;        // label column width
	const int BW  = 240;       // bar width
	const int BH  = CH - 4;    // bar height
	const int PW  = 600;       // panel width

	// colors
	const std::uint32_t C_BG     = 0xFF111111;
	const std::uint32_t C_SEP    = 0xFF444444;
	const std::uint32_t C_BBKG   = 0xFF333333;  // bar background
	const std::uint32_t C_HP_HI  = 0xFF00CC44;
	const std::uint32_t C_HP_MID = 0xFFFF8800;
	const std::uint32_t C_HP_LO  = 0xFFFF2222;
	const std::uint32_t C_ARMOR  = 0xFF3399FF;
	const std::uint32_t C_AMMO   = 0xFFFFDD00;
	const std::uint32_t C_CD     = 0xFFBBBBBB;  // cooldown fill
	const std::uint32_t C_RED    = 0xFFCC0000;
	const std::uint32_t C_RELOAD = 0xFF441111;  // reloading row bg

	// Draw a filled bar [background + colored fill]
	auto draw_bar = [&](int x, int y, int w, int h, float cur, float max,
	                     std::uint32_t color) {
		r2d.draw_rect(x, y, w, h, C_BBKG);
		if (max > 0.0f) {
			int fill = static_cast<int>(std::min(1.0f, cur / max) * w);
			if (fill > 0) r2d.draw_rect(x, y, fill, h, color);
		}
	};

	auto sep = [&](int y) { r2d.draw_rect(PX, y, PW, 2, C_SEP); };

	// ---- weapon pool ----
	std::vector<weapon_slot> pool;

	auto add_slot = [&](auto wptr, const std::string& name,
	                     int reserve, float reload_dur, bool is_auto = false) {
		pool.push_back({ std::move(wptr), name, reserve, 0.0f, reload_dur,
		                 false, false, is_auto });
	};

	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_slot(std::make_unique<engine::combat::pistol>(std::move(a)),       "Pistol",    5, 1.5f);       }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_slot(std::make_unique<engine::combat::smg>(std::move(a)),          "SMG",       4, 2.5f, true); }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_slot(std::make_unique<engine::combat::rifle>(std::move(a)),        "Rifle",     4, 2.0f, true); }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_slot(std::make_unique<engine::combat::shotgun>(std::move(a)),      "Shotgun",   4, 3.0f);       }
	{ auto a = std::make_unique<engine::combat::bullet_ammunition>();
	  add_slot(std::make_unique<engine::combat::sniper_rifle>(std::move(a)), "Sniper",    3, 3.5f);       }
	{ auto a = std::make_unique<engine::combat::plasma_ammunition>();
	  add_slot(std::make_unique<engine::combat::plasma_gun>(std::move(a)),   "Plasma",    3, 2.0f);       }
	pool.push_back({ std::make_unique<engine::combat::katana>(),
	                 "Katana", 0, 0.0f, 0.0f, false, true, false });

	pool[0].in_loadout = true;

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
			for (int i = 0; i < (int)p.weapons.size(); ++i)
				if (p.weapons[i] == preserve) { p.switch_weapons(i); return; }
			p.switch_weapons(0);
		}
	};
	rebuild_loadout();

	// ---- input backend ----
	auto i_back = std::make_unique<input::evdev::backend>();
	if (i_back->is_bad()) {
		std::cerr << "error: failed to initialize input backend\n";
		return 1;
	}

	cmd_mode mode = cmd_mode::none;
	std::string num_buf;

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
		bool cur   = i_back->is_key_down(k);
		bool fired = cur && !prev;
		prev       = cur;
		return fired;
	};

	auto last = std::chrono::steady_clock::now();

	while (true) {
		auto now = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(now - last).count();
		last = now;

		i_back->update();
		if (i_back->is_bad()) break;

		bool game_over = p.dead();

		if (i_back->is_key_down(input::key::esc)) {
			if (mode != cmd_mode::none) { mode = cmd_mode::none; num_buf.clear(); }
			else break;
		}

		weapon_slot* cur_slot = find_slot(pool, p.current_weapon);

		if (game_over) {
			if (edge(input::key::v, prev_v))
				p.heal(p.max_hp());
		} else {
			// Weapon switch 1-9 (disabled while entering a number)
			if (mode == cmd_mode::none) {
				for (int i = 1; i <= 9; ++i) {
					bool down = i_back->is_key_down(numkeys[i]);
					if (down && !prev_num[i]) p.switch_weapons(i - 1);
					prev_num[i] = down;
				}
				cur_slot = find_slot(pool, p.current_weapon);
			}

			// Shoot
			{
				auto mouse = i_back->get_mouse_state();
				bool reloading = cur_slot && cur_slot->reload_timer > 0.0f;
				bool triggered = cur_slot && cur_slot->is_auto
				               ? mouse.left : (mouse.left && !prev_lmb);
				if (!reloading && triggered) p.shoot();
				prev_lmb = mouse.left;
			}

			// Reload (R, edge trigger, costs one reserve mag)
			bool r_down = i_back->is_key_down(input::key::r);
			if (r_down && !prev_r && cur_slot
			    && !cur_slot->is_melee
			    && cur_slot->reserve_mags > 0
			    && cur_slot->reload_timer <= 0.0f)
			{
				--cur_slot->reserve_mags;
				cur_slot->reload_timer = cur_slot->reload_duration;
			}
			prev_r = r_down;

			// Add / remove weapon
			bool plus_down = i_back->is_key_down(input::key::equals);
			if (plus_down && !prev_plus)
				for (auto& s : pool)
					if (!s.in_loadout) { s.in_loadout = true; rebuild_loadout(); break; }
			prev_plus = plus_down;

			bool minus_down = i_back->is_key_down(input::key::hyphen);
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

			// Number / Backspace / Enter while in command mode
			if (mode != cmd_mode::none) {
				for (int i = 0; i < 10; ++i) {
					bool down = i_back->is_key_down(numkeys[i]);
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

		p.update(dt);

		// ==== RENDER FRAME ====
		cur_slot = find_slot(pool, p.current_weapon);

		// Clear screen
		std::memset(r_back->get_mmio(), 0,
		            static_cast<std::size_t>(r_back->get_height()) * r_back->get_pitch());

		// Panel background
		r2d.draw_rect(PX - 8, PY - 8, PW + 16, 420, C_BG);

		int y = PY;

		// Header
		r2d.draw_text(game_over ? "=== GAME  OVER  ===" : "=== PLAYER STATUS ===", PX, y, CW, CH);
		y += LS + 2;

		// HP bar
		{
			float ratio = (p.max_hp() > 0) ? p.hp() / p.max_hp() : 0.0f;
			std::uint32_t c = ratio > 0.6f ? C_HP_HI : (ratio > 0.3f ? C_HP_MID : C_HP_LO);
			r2d.draw_text("HP:", PX, y + 2, CW, CH - 4);
			draw_bar(PX + LBL, y, BW, BH, p.hp(), p.max_hp(), c);
			r2d.draw_text(fmt1(p.hp()) + " / " + fmt1(p.max_hp()),
			              PX + LBL + BW + 8, y + 2, CW - 2, CH - 4);
			y += LS;
		}

		// Armor bar
		{
			r2d.draw_text("AR:", PX, y + 2, CW, CH - 4);
			draw_bar(PX + LBL, y, BW, BH, p.armor(), p.max_armor(), C_ARMOR);
			r2d.draw_text(fmt1(p.armor()), PX + LBL + BW + 8, y + 2, CW - 2, CH - 4);
			y += LS;
		}

		// FX line
		if (p.effect_count() > 0)
			r2d.draw_rect(PX, y, 162, CH, 0xFF331100),
			r2d.draw_text("FX: BURNING", PX + 4, y, CW, CH);
		y += LS;

		sep(y); y += 10;

		// Weapon section
		if (p.current_weapon && cur_slot) {
			std::string wlabel = cur_slot->name
			    + (cur_slot->is_auto ? " [AUTO]" : "")
			    + "  (" + std::to_string(p.current_weapon_index + 1)
			    + "/" + std::to_string((int)p.weapons.size()) + ")";
			r2d.draw_text(wlabel, PX, y, CW, CH);
			y += LS;

			if (cur_slot->is_melee) {
				r2d.draw_text("AMMO: --- (melee)", PX, y, CW, CH); y += LS;
				r2d.draw_text("MAGS: ---",         PX, y, CW, CH); y += LS;
				// swing cooldown
				float lst    = p.current_weapon->last_shot_time;
				float max_cd = 1.0f / p.current_weapon->fire_rate;
				if (lst > 0.005f) {
					r2d.draw_text("Swing: " + fmt1(lst) + "s", PX, y + 2, CW, CH - 4);
					draw_bar(PX + 165, y, 120, BH, 1.0f - lst / max_cd, 1.0f, C_CD);
				} else {
					r2d.draw_text("Ready", PX, y, CW, CH);
				}
				y += LS;

			} else if (cur_slot->reload_timer > 0.0f) {
				r2d.draw_text("AMMO: " + std::to_string(p.current_weapon->ammo_count)
				              + " / " + std::to_string(p.current_weapon->max_ammo),
				              PX, y, CW, CH);
				y += LS;
				r2d.draw_text("MAGS: " + std::to_string(cur_slot->reserve_mags) + " remaining",
				              PX, y, CW, CH);
				y += LS;
				r2d.draw_rect(PX, y - 2, PW, CH + 6, C_RELOAD);
				r2d.draw_text("RELOADING...  " + fmt1(cur_slot->reload_timer) + "s",
				              PX + 4, y, CW, CH);
				// reload progress bar
				draw_bar(PX + 280, y + 2, 180, BH - 2,
				         cur_slot->reload_duration - cur_slot->reload_timer,
				         cur_slot->reload_duration, C_CD);
				y += LS;

			} else {
				// Ammo bar
				r2d.draw_text("AMMO:", PX, y + 2, CW, CH - 4);
				draw_bar(PX + LBL, y, BW, BH,
				         static_cast<float>(p.current_weapon->ammo_count),
				         static_cast<float>(p.current_weapon->max_ammo), C_AMMO);
				r2d.draw_text(std::to_string(p.current_weapon->ammo_count)
				              + " / " + std::to_string(p.current_weapon->max_ammo),
				              PX + LBL + BW + 8, y + 2, CW - 2, CH - 4);
				y += LS;

				r2d.draw_text("MAGS: " + std::to_string(cur_slot->reserve_mags) + " remaining",
				              PX, y, CW, CH);
				y += LS;

				// Shot cooldown (semi-auto only)
				if (!cur_slot->is_auto) {
					float lst    = p.current_weapon->last_shot_time;
					float max_cd = 1.0f / p.current_weapon->fire_rate;
					if (lst > 0.005f) {
						r2d.draw_text("Shot: " + fmt1(lst) + "s", PX, y + 2, CW, CH - 4);
						draw_bar(PX + 155, y, 160, BH, 1.0f - lst / max_cd, 1.0f, C_CD);
					} else if (p.current_weapon->ammo_count == 0 && cur_slot->reserve_mags > 0) {
						r2d.draw_text("[R] to reload", PX, y, CW, CH);
					} else {
						r2d.draw_text("Ready", PX, y, CW, CH);
					}
					y += LS;
				} else {
					y += LS;
				}
			}
		} else {
			r2d.draw_text("Weapon: (none)", PX, y, CW, CH); y += LS + LS + LS;
		}

		sep(y); y += 10;

		// Loadout — two rows of up to 4 weapons each, small font
		{
			std::string rows[2];
			for (int i = 0; i < (int)p.weapons.size(); ++i) {
				bool active = (p.current_weapon && i == p.current_weapon_index);
				std::string n;
				for (auto& s : pool)
					if (s.in_loadout && s.owned.get() == p.weapons[i]) { n = s.name; break; }
				if ((int)n.size() > 7) n = n.substr(0, 7);
				std::string entry = std::to_string(i + 1) + ":"
				    + (active ? "[" : "") + n + (active ? "] " : " ");
				rows[i < 4 ? 0 : 1] += entry;
			}
			if (p.weapons.empty()) rows[0] = "(empty)";
			r2d.draw_text("LOAD: " + rows[0], PX, y, SCW, SCH);
			y += SCH + 4;
			if (!rows[1].empty()) {
				r2d.draw_text("      " + rows[1], PX, y, SCW, SCH);
				y += SCH + 4;
			}
		}

		sep(y); y += 10;

		// Controls or command prompt
		if (game_over) {
			// Centered red banner
			const int BNW = 480, BNH = 80;
			const int BNX = SW / 2 - BNW / 2, BNY = SH / 2 - BNH / 2;
			r2d.draw_rect(BNX, BNY, BNW, BNH, C_RED);
			r2d.draw_text("GAME  OVER", BNX + 30, BNY + 14, 34, 50);
			r2d.draw_text("[V] Revive          [ESC] Exit",
			              BNX + 30, BNY + BNH + 12, CW, CH);
		} else if (mode != cmd_mode::none) {
			const char* prompt =
				(mode == cmd_mode::damage) ? "Damage amount" :
				(mode == cmd_mode::heal)   ? "Heal amount"   : "Armor amount";
			r2d.draw_rect(0, SH - 70, SW, 70, 0xFF1A1A2A);
			r2d.draw_rect(0, SH - 70, SW, 2, 0xFF5555AA);
			r2d.draw_text(std::string("> ") + prompt + ": " + num_buf + "_",
			              12, SH - 58, CW, CH);
			r2d.draw_text("[0-9] type   [Backspace] delete   [Enter] apply   [ESC] cancel",
			              12, SH - 30, CW - 2, CH - 4);
		} else {
			r2d.draw_text("1-9: switch   LMB: shoot   R: reload",        PX, y, CW, CH); y += LS;
			r2d.draw_text("=: add weapon   -: remove   ESC: exit",       PX, y, CW, CH); y += LS;
			r2d.draw_text("D: damage   H: heal   A: armor   B: burn(5s)", PX, y, CW, CH);
		}

		r_back->flush();
		r_back->wait_for_vsync();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return 0;
}
