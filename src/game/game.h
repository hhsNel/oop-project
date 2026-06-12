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
#include "entities/player.h"

namespace game {
	class game {
		/* used resource loader */
		util::resource_loader rl;
		/* used asset manager */
		assets::asset_manager am;
		/* used input backend */
		std::unique_ptr<input::input_backend> input;
		/* used rendering backend */
		std::unique_ptr<rendering::rendering_backend> rb;
		/* used audio backend */
		std::unique_ptr<audio::audio_backend> ab;
		/* map */
		geometry::map_data md;
		/* audio mixer */
		audio::audio_mixer mix;
		/* 2d renderer (eg. for HUD) */
		rendering::renderer_2d r2d;
		/* 3d renderer */
		rendering::software_renderer sr;
		/* world */
		engine::world w;
		/* player pointer */
		entities::player* player_ptr;

		/* player fov */
		float fov;

		/* display a star wars like text scroll */
		void text_scroll(std::vector<std::string> const& text, assets::audio_clip_id const aid);
		/* display a text scroll from a resource */
		void ts_from_resource(std::string const& res_name, assets::audio_clip_id const aid);
		/* display the opening */
		void opening();
		/* display the ending */
		void ending();
		/* display the credits */
		void credits();
		/* show the main menu */
		void show_main_menu();
		/* show the options menu */
		void show_options();
		/* game loop */
		void loop();
		/* overlay the hud */
		void draw_hud();
	public:
		/* constructor */
		game();
		/* run game */
		void run();
	};
}
