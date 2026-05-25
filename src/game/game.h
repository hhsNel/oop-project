#pragma once

#include <vector>
#include <memory>

#include "util/resource-loader.h"
#include "assets/asset-manager.h"
#include "input/input-backend.h"
#include "rendering/rendering-backend.h"
#include "audio/audio-backend.h"
#include "geometry/map-data.h"
#include "engine/world.h"
#include "audio/audio-mixer.h"
#include "rendering/renderer-2d.h"
#include "rendering/software-renderer.h"

namespace game {
	class game {
		util::resource_loader rl;
		assets::asset_manager am;
		std::unique_ptr<input::input_backend> input;
		std::unique_ptr<rendering::rendering_backend> rb;
		std::unique_ptr<audio::audio_backend> ab;
		geometry::map_data md;
		audio::audio_mixer mix;
		rendering::renderer_2d r2d;
		rendering::software_renderer sr;
		engine::world w;

		float fov;

		void text_scroll(std::vector<std::string> const& text, assets::audio_clip_id const aid);
		void ts_from_resource(std::string const& res_name, assets::audio_clip_id const aid);
		void opening();
		void ending();
		void credits();
		void show_main_menu();
		void show_options();
	public:
		game();
		void run();
	};
}
