#include "game.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>

#include "rendering/drm-kms/backend.h"
#include "audio/alsa/backend.h"
#include "input/evdev-backend.h"

namespace game {

	game::game() :
		rl{},
		am(assets::asset_manager::load(rl)),
		input(std::make_unique<input::evdev::backend>()),
		rb(std::make_unique<rendering::drm_kms::backend>()),
		ab(std::make_unique<audio::alsa::backend>()),
		md{},
		mix(*ab, audio::audio_format{44100, 2, 16}),
		r2d(*rb, am, am.flat_tx_by_id(0)),
		sr(*rb, am, md),
		w{} {
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

			mix.step(static_cast<int>(44100 * dt_secs));
			rb->wait_for_vsync();

			scroll_offset += 90 * dt_secs;
		}
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

	void game::run() {
		ts_from_resource("opening-scroll", 0);
	}

}
