#include "game.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <numbers>

#include "rendering/drm-kms/backend.h"
#include "audio/alsa/backend.h"
#include "input/evdev/backend.h"
#include "entities/monster-factory.h"
#include "world_objects/pickup-factory.h"

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

		int const horizon_y    = screen_h / 4;
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

				float scale  = (flat_y - horizon_y) / static_cast<float>(screen_h - horizon_y);
				int   char_w = static_cast<int>(base_char_w * scale);
				int   char_h = static_cast<int>(base_char_h * scale);

				if (char_w == 0 || char_h == 0)
					continue;

				int draw_x = (screen_w - static_cast<int>(text[i].length()) * char_w) / 2;
				int draw_y = static_cast<int>(flat_y);

				r2d.draw_text(text[i].c_str(), draw_x, draw_y, char_w, char_h, 0x007fff);
			}

			rb->flush();

			auto now      = std::chrono::high_resolution_clock::now();
			float dt_secs = std::chrono::duration<float>(now - last_tick).count();
			last_tick     = now;

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
			if (nl == std::string_view::npos)
				nl = content.size();
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
			math::vec2{256.0f, 128.0f}, 0.0f, 0, 1.0f,
			100.0f, 50.0f, 120.0f, 1.0f);
		player_ptr = &*p;
		w.register_entity(std::move(p));

		bool has_bsp = md.root_node_id != util::indexed_storage<geometry::bsp_node>::nullid;

		for (auto const& spawn : md.monster_spawns) {
			auto m = entities::make_monster(spawn("type"_f), spawn("pos"_f), spawn("z"_f));
			if (m) {
				auto sub_id = has_bsp ? md.get_subsector_id(spawn("pos"_f)) : 1;
				md.subsectors[sub_id].add_sprite(std::move(m));
			}
		}

		for (auto const& spawn : md.pickup_spawns) {
			auto sub_id = has_bsp ? md.get_subsector_id(spawn("pos"_f)) : 1;
			auto pk = world_object::make_pickup(
				spawn("type"_f), spawn("subtype"_f),
				spawn("pos"_f), spawn("z"_f),
				*player_ptr, md, sub_id, w);
			if (pk)
				md.subsectors[sub_id].add_sprite(std::move(pk));
		}

		int prev_mouse_x = 0;
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

			// shooting
			if (mouse.left)
				player_ptr->shoot();
			if (input->is_key_down(input::key::r))
				player_ptr->reload();

			w.update(dt);

			// render
			auto& spr = static_cast<util::componentized<rendering::sprite>&>(*player_ptr);
			std::memset(const_cast<std::uint32_t*>((*rb)("mmio"_f)), 0x00,
				static_cast<std::size_t>((*rb)("height"_f)) * (*rb)("pitch"_f));

			sr.render_bsp(spr("pos"_f), CAM_HEIGHT, spr("angle"_f), fov);

			rb->flush();

			mix.step(static_cast<int>(48000.0f * dt));

			rb->wait_for_vsync();
		}
	}

	void game::run() {
//		opening();
		show_main_menu();
	}

}
