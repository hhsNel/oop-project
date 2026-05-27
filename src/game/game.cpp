#include "game.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <cmath>
<<<<<<< HEAD
=======
#include <numbers>
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361

#include "rendering/drm-kms/backend.h"
#include "audio/alsa/backend.h"
#include "input/evdev/backend.h"
#include "entities/monster-factory.h"
<<<<<<< HEAD
#include "entities/monsters/monster-boss.h"
=======
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
#include "world_objects/pickup-factory.h"
#include "combat/weapons/pistol.h"


//changes before rebase


namespace game {

	game::game() :
		rl{},
		am(assets::asset_manager::load(rl)),
		input(std::make_unique<input::evdev::backend>()),
		rb(std::make_unique<rendering::drm_kms::backend>()),
		ab(std::make_unique<audio::alsa::backend>()),
		md(geometry::map_data::load_from_bin(
			*rl.lookup_resource("map/sectors.bin"),
			*rl.lookup_resource("map/sidedefs.bin"),
			*rl.lookup_resource("map/linedefs.bin"),
			*rl.lookup_resource("map/subsectors.bin"),
			*rl.lookup_resource("map/nodes.bin"),
			*rl.lookup_resource("map/monsters.bin"),
			*rl.lookup_resource("map/pickups.bin"))),
		mix(*ab, audio::audio_format{48000, 2, 16}),
		r2d(*rb, am, am.ui_tx_by_id(0)),
		sr(*rb, am, md),
		w(),
		player_ptr(nullptr),

		fov(1.55) {
		if (input->is_bad()) {
			std::cerr << "error: failed to initialize input backend\n";
			std::exit(1);
		}

		if (rb->bad()) {
			std::cerr << "error: failed to initialize rendering backend\n";
			std::exit(1);
		}

		auto modes = (*rb)("modes"_f);
		if (modes.empty()) {
			std::cerr << "error: no rendering modes available\n";
			std::exit(1);
		}
		rb->push_mode(std::move(modes[0]));

		if (rb->bad()) {
			std::cerr << "error: rendering backend entered bad state after mode set\n";
			std::exit(1);
		}

		if (ab->bad()) {
			std::cerr << "error: failed to initialize audio backend\n";
			std::exit(1);
		}
	}

	void game::text_scroll(std::vector<std::string> const& text, assets::audio_clip_id const aid) {
		int const screen_w = (*rb)("width"_f);
		int const screen_h = (*rb)("height"_f);

		int const horizon_y    = screen_h / 8;
		int const base_char_w  = 32;
		int const base_char_h  = 48;
		int const line_spacing = 60;

		float const total_scroll = screen_h + static_cast<float>(text.size() * line_spacing) + horizon_y;
		float scroll_offset = 0.0f;

		mix.play(am.audio_clip_by_id(aid));

		auto last_tick = std::chrono::high_resolution_clock::now();

		while (scroll_offset < total_scroll) {
			float last_line_flat_y = screen_h - scroll_offset + static_cast<float>((text.size() - 1) * line_spacing);
			if (last_line_flat_y <= horizon_y) break;

			std::memset(const_cast<std::uint32_t *>((*rb)("mmio"_f)), 0x00, screen_h * (*rb)("pitch"_f));

			for (std::size_t i = 0; i < text.size(); ++i) {
				float flat_y = screen_h - scroll_offset + static_cast<float>(i * line_spacing);

				if (flat_y <= horizon_y || flat_y > screen_h + line_spacing)
					continue;

				float time = (flat_y - horizon_y) / static_cast<float>(screen_h - horizon_y);
				float scale = std::pow(time, 0.6666f);
				int char_w = static_cast<int>(base_char_w * scale);
				int char_h = static_cast<int>(base_char_h * scale);

				if (char_w == 0 || char_h == 0) continue;

				int draw_x = (screen_w - static_cast<int>(text[i].length()) * char_w) / 2;
				int draw_y = static_cast<int>(flat_y);

				r2d.draw_text(text[i].c_str(), draw_x, draw_y, char_w, char_h, 0x007fff);
			}

			rb->flush();

			auto now = std::chrono::high_resolution_clock::now();
			float dt_secs = std::chrono::duration<float>(now - last_tick).count();
			last_tick = now;

			mix.step(static_cast<int>(48000 * dt_secs));
			rb->wait_for_vsync();

			scroll_offset += 90 * dt_secs;
		}

		mix.stop_all();
	}

	void game::ts_from_resource(std::string const& res_name, assets::audio_clip_id const aid) {
		util::resource const* res = rl.lookup_resource(res_name);
		if (!res) {
			std::cerr << "error: resource not found: " << res_name << "\n";
			return;
		}

		std::string_view content(static_cast<char const*>((*res)("beginning"_f)), (*res)("size"_f));

		std::vector<std::string> lines;
		std::string_view::size_type pos = 0;
		while (pos < content.size()) {
			auto nl = content.find('\n', pos);
			if (nl == std::string_view::npos) nl = content.size();
			lines.emplace_back(content.substr(pos, nl - pos));
			pos = nl + 1;
		}

		text_scroll(lines, aid);
	}

	void game::credits() {
		ts_from_resource("credits", 0);
	}

	void game::opening() {
		ts_from_resource("opening-scroll", 0);
	}

	void game::ending() {
		ts_from_resource("ending-scroll", 0);
	}

	void game::show_main_menu() {
		while(1) {
			int result = am.display_menu(0, r2d, *rb, *input);

			switch (result) {
				case -1:
					return;
				case 0:
					loop();
					break;
				case 1:
					credits();
					break;
				case 2:
					show_options();
					break;
				default:
					break;
			}
		}
	}

	void game::show_options() {
		auto modes = (*rb)("modes"_f);
		std::size_t mode_idx = 0;

		while(1) {
			assets::menu& menu = am.menu_by_id(1);

			menu.selective_formatter(
				"{mode}|{fov}",
				[&](std::string_view tmpl) {
					struct replacement { std::string_view marker; std::string value; };
					replacement replacements[] = {
						{ "{mode}", modes.empty() ? "N/A" :
							std::to_string((*modes[mode_idx])("x_res"_f))+"x"+std::to_string((*modes[mode_idx])("y_res"_f))+" "+std::to_string((*modes[mode_idx])("refresh_hz"_f))+"Hz" },
						{ "{fov}", std::to_string(fov) },
					};

					std::string result(tmpl);
					for (auto const& r : replacements) {
						for (std::size_t pos = 0;
							 (pos = result.find(r.marker, pos)) != std::string::npos;) {
							result.replace(pos, r.marker.size(), r.value);
							pos += r.value.size();
						}
					}
					return result;
				}
			);

			int result = am.display_menu(1, r2d, *rb, *input);

			switch (result) {
				case -1:
					return;
				case 0:
					am.cycle_set();
					break;
				case 1:
					if (!modes.empty()) {
						mode_idx = (mode_idx + modes.size() - 1) % modes.size();
						rb->push_mode(std::move(modes[mode_idx]));
						modes = (*rb)("modes"_f);
					}
					break;
				case 2:
					if (!modes.empty()) {
						mode_idx = (mode_idx + 1) % modes.size();
						rb->push_mode(std::move(modes[mode_idx]));
						modes = (*rb)("modes"_f);
					}
					break;
				case 3:
					if (fov > 0.5) fov -= 0.05;
					break;
				case 4:
					if (fov < 3) fov += 0.05;
					break;
				default:
					break;
			}
		}
	}

	void game::loop() {
		constexpr float CAM_HEIGHT  = 48.0f;
		constexpr float MOUSE_SENS  = 0.002f;

		auto p = std::make_unique<entities::player>(
			math::vec2{0.0f, 0.0f}, 0.0f, 0, 1.0f,
			100.0f, 50.0f, 120.0f, 1.0f);
		player_ptr = &*p;

		// Give player a starting pistol (slot 0)
		player_ptr->weapons.resize(7);
<<<<<<< HEAD
		player_ptr->weapons[0] = std::make_unique<combat::weapons::pistol>(md, w, mix, am);
		player_ptr->current_weapon_index = 0;
		player_ptr->map_ref = &md;
		player_ptr->world_ref = &w;
=======
		player_ptr->weapons[0] = std::make_unique<combat::weapons::pistol>(md, w);
		player_ptr->current_weapon_index = 0;
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361

		w.register_entity(std::move(p));

		bool has_bsp = md.root_node_id != util::indexed_storage<geometry::bsp_node>::nullid;

		std::vector<entities::monster*> alive_monsters;

		for (auto const& spawn : md.monster_spawns) {
			auto m = entities::make_monster(spawn("type"_f), spawn("pos"_f), spawn("z"_f));
			if (m) {
				auto* raw = &*m;
				raw->target_ptr = player_ptr;
				raw->map_ref = &md;
<<<<<<< HEAD
				raw->world_ref = &w;
				if (auto* boss = dynamic_cast<entities::monster_boss*>(raw)) {
					boss->boss_map_ref = &md;
				}
=======
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
				auto sub_id = has_bsp ? md.get_subsector_id(spawn("pos"_f)) : 1;
				w.register_entity(std::move(m));
				md.subsectors[sub_id].add_sprite(raw);
				alive_monsters.push_back(raw);
			}
		}

		for (auto const& spawn : md.pickup_spawns) {
			auto sub_id = has_bsp ? md.get_subsector_id(spawn("pos"_f)) : 1;
			auto pk = world_object::make_pickup(
				spawn("type"_f), spawn("subtype"_f),
				spawn("pos"_f), spawn("z"_f),
<<<<<<< HEAD
				*player_ptr, md, sub_id, w, mix, am);
=======
				*player_ptr, md, sub_id, w);
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
			if (pk) {
				auto* raw = &*pk;
				w.register_entity(std::move(pk));
				md.subsectors[sub_id].add_sprite(raw);
			}
		}

		int prev_mouse_x = 0;
		bool prev_r = false;
		auto last_tick = std::chrono::high_resolution_clock::now();

		while(1) {
			auto now = std::chrono::high_resolution_clock::now();
			float dt = std::chrono::duration<float>(now - last_tick).count();
			last_tick = now;

			if (dt > 0.5f) dt = 0.5f;

			input->update();
			if (input->is_key_down(input::key::esc)) break;

			// movement
			float fwd = 0.0f, strafe = 0.0f;
			if (input->is_key_down(input::key::w)) strafe -= dt;
			if (input->is_key_down(input::key::s)) strafe += dt;
			if (input->is_key_down(input::key::a)) fwd    -= dt;
			if (input->is_key_down(input::key::d)) fwd    += dt;
			if (fwd != 0.0f || strafe != 0.0f)
				player_ptr->move({strafe, fwd});

			// mouse rotation
			auto mouse   = input->get_mouse_state();
			int delta_x  = mouse.x - prev_mouse_x;
			prev_mouse_x = mouse.x;
			if (delta_x != 0)
				player_ptr->rotate(static_cast<float>(-delta_x) * MOUSE_SENS);

			// weapon switching (1-7)
			if (input->is_key_down(input::key::n1)) player_ptr->switch_weapons(0);
			if (input->is_key_down(input::key::n2)) player_ptr->switch_weapons(1);
			if (input->is_key_down(input::key::n3)) player_ptr->switch_weapons(2);
			if (input->is_key_down(input::key::n4)) player_ptr->switch_weapons(3);
			if (input->is_key_down(input::key::n5)) player_ptr->switch_weapons(4);
			if (input->is_key_down(input::key::n6)) player_ptr->switch_weapons(5);
			if (input->is_key_down(input::key::n7)) player_ptr->switch_weapons(6);

			// shooting
			if (mouse.left)
				player_ptr->shoot();
			bool cur_r = input->is_key_down(input::key::r);
			if (cur_r && !prev_r)
				player_ptr->reload();
			prev_r = cur_r;

			w.update(dt);

			// remove dead monsters from subsectors (stop rendering them)
			for (auto it = alive_monsters.begin(); it != alive_monsters.end(); ) {
				if ((*it)->is_dead()) {
					auto* spr = static_cast<rendering::sprite*>(*it);
					auto sub_id = has_bsp ? md.get_subsector_id((*spr)("pos"_f)) : 1;
					md.subsectors[sub_id].remove_sprite(spr);
					it = alive_monsters.erase(it);
				} else {
					++it;
				}
			}

			// render
			auto& spr = static_cast<util::componentized<rendering::sprite>&>(*player_ptr);
			std::memset(const_cast<std::uint32_t*>((*rb)("mmio"_f)), 0x00,
				static_cast<std::size_t>((*rb)("height"_f)) * (*rb)("pitch"_f));

			sr.render_bsp(spr("pos"_f), CAM_HEIGHT, spr("angle"_f), fov);

			draw_hud();

			rb->flush();

			mix.step(static_cast<int>(48000.0f * dt));

			rb->wait_for_vsync();
		}
	}

	void game::draw_hud() {
		if (!player_ptr) return;

		int const sw = (*rb)("width"_f);
		int const sh = (*rb)("height"_f);

		// HUD element scale (4x)
		constexpr int SCALE = 4;

<<<<<<< HEAD
		// ── Crosshair (center of screen) ──────────────────────────────
		auto const& ch_tex = am.ui_tx_by_id(9);  // crosshair2.btx
		int const ch_size = 32 * 2;  // 64px on screen
		r2d.draw_texture(ch_tex, sw / 2 - ch_size / 2, sh / 2 - ch_size / 2, ch_size, ch_size);

=======
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
		// ── HP / Armor HUD (left bottom) ──────────────────────────────
		auto const& hp_tex = am.ui_tx_by_id(6);  // HP-Arm-HUD.btx
		int const hp_w = hp_tex("width"_f) * SCALE;   // 64*4 = 256
		int const hp_h = hp_tex("height"_f) * SCALE;  // 32*4 = 128
		int const hp_x = 0;
		int const hp_y = sh - hp_h;

		r2d.draw_texture(hp_tex, hp_x, hp_y, hp_w, hp_h);

		// HP bar: to the right of the heart icon
		float hp_frac = player_ptr->health("current_hp"_f) / player_ptr->health("max_hp"_f);
		if (hp_frac < 0.0f) hp_frac = 0.0f;
		if (hp_frac > 1.0f) hp_frac = 1.0f;
		int const bar_max_w = 46 * SCALE;
		int const bar_h = 10 * SCALE;
		int const hp_bar_x = hp_x + 18 * SCALE;
		int const hp_bar_y = hp_y + 3 * SCALE;
		r2d.draw_rect(hp_bar_x, hp_bar_y, static_cast<int>(bar_max_w * hp_frac), bar_h, 0xFFFF0000);

		// Armor bar: to the right of the shield icon
		float arm_frac = 0.0f;
		if (player_ptr->health("max_armor"_f) > 0.0f)
			arm_frac = player_ptr->health("armor"_f) / player_ptr->health("max_armor"_f);
		if (arm_frac < 0.0f) arm_frac = 0.0f;
		if (arm_frac > 1.0f) arm_frac = 1.0f;
		int const arm_bar_x = hp_x + 18 * SCALE;
		int const arm_bar_y = hp_y + 19 * SCALE;
		r2d.draw_rect(arm_bar_x, arm_bar_y, static_cast<int>(bar_max_w * arm_frac), bar_h, 0xFF4488FF);

		// ── Mag HUD (right bottom) ────────────────────────────────────
		auto const& mag_tex = am.ui_tx_by_id(7);  // Mag_HUD.btx
		int const mag_w = mag_tex("width"_f) * SCALE;   // 32*4 = 128
		int const mag_h = mag_tex("height"_f) * SCALE;  // 32*4 = 128
		int const mag_x = sw - mag_w;
		int const mag_y = sh - mag_h;

		r2d.draw_texture(mag_tex, mag_x, mag_y, mag_w, mag_h);

		// Draw ammo count and reserve mags
		if (player_ptr->current_weapon_index >= 0 &&
			player_ptr->current_weapon_index < static_cast<int>(player_ptr->weapons.size())) {
			auto const& wpn = player_ptr->weapons[player_ptr->current_weapon_index];
			if (wpn) {
				std::string ammo_str = std::to_string(wpn->ammo_count);
				std::string mags_str = std::to_string(wpn->reserve_mags);
<<<<<<< HEAD
				// Large ammo count in upper half
				int const ammo_cw = 20;
				int const ammo_ch = 28;
				int ammo_tx = mag_x + mag_w / 2 - static_cast<int>(ammo_str.size()) * ammo_cw / 2;
				int ammo_ty = mag_y + 2 * SCALE;
				r2d.draw_text(ammo_str, ammo_tx, ammo_ty, ammo_cw, ammo_ch, 0xFFFFFF);
				// Smaller reserve count in lower half
				int const mag_cw = 14;
				int const mag_ch = 20;
				int mags_tx = mag_x + mag_w / 2 - static_cast<int>(mags_str.size()) * mag_cw / 2;
				int mags_ty = mag_y + mag_h - mag_ch - 3 * SCALE;
				r2d.draw_text(mags_str, mags_tx, mags_ty, mag_cw, mag_ch, 0xCCCCCC);
=======
				int const char_w = 12;
				int const char_h = 16;
				// Ammo count centered in upper area
				r2d.draw_text(ammo_str, mag_x + mag_w / 2 - static_cast<int>(ammo_str.size()) * char_w / 2, mag_y + 4 * SCALE, char_w, char_h, 0xFFFFFF);
				// Reserve mags below
				r2d.draw_text(mags_str, mag_x + mag_w / 2 - static_cast<int>(mags_str.size()) * char_w / 2, mag_y + mag_h - char_h - 2 * SCALE, char_w, char_h, 0xCCCCCC);
>>>>>>> 118b11eca7c34dd1debab28313e8436d01ca5361
			}
		}

		// ── Weapon Inventory (left of Mag HUD) ───────────────────────
		auto const& inv_tex = am.ui_tx_by_id(8);  // Weapon_inventory.btx
		int const inv_w = inv_tex("width"_f) * SCALE;   // 64*4 = 256
		int const inv_h = inv_tex("height"_f) * SCALE;  // 32*4 = 128
		int const inv_x = mag_x - inv_w;
		int const inv_y = sh - inv_h;

		r2d.draw_texture(inv_tex, inv_x, inv_y, inv_w, inv_h);

		// Draw weapon sprites next to their slot numbers
		// Texture layout: 2 columns x 4 rows, numbers 1-4 left column, 5-7 right column
		// Each cell is ~32x8 pixels in the 64x32 source
		int const cell_w = 32 * SCALE;
		int const cell_h = 8 * SCALE;
		int const sprite_size = 6 * SCALE;

		for (int i = 0; i < static_cast<int>(player_ptr->weapons.size()) && i < 7; ++i) {
			if (!player_ptr->weapons[i]) continue;
			int col = i / 4;
			int row = i % 4;
			int slot_x = inv_x + col * cell_w + 10 * SCALE;
			int slot_y = inv_y + row * cell_h;
			// weapon sprite IDs: 15=pistol, 16=smg, 17=rifle, 18=shotgun, 19=sniper, 20=plasma, 21=katana
			auto const& weapon_sprite = am.sprite_tx_by_id(15 + i);
			r2d.draw_texture(weapon_sprite, slot_x, slot_y + 1 * SCALE, sprite_size, sprite_size);
			// Highlight current weapon
			if (i == player_ptr->current_weapon_index) {
				r2d.draw_rect(slot_x - 1, slot_y, sprite_size + 2, cell_h, 0x80FFFF00);
			}
		}
	}

	void game::run() {
//		opening();
		show_main_menu();
	}

}
