#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>
#include <cstring>

#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"
#include "graphics/texture-manager.h"
#include "graphics/texture.h"
#include "util/resource-loader.h"

bool check_backend_state(const std::unique_ptr<rendering::rendering_backend>& backend, const std::string& action) {
    if (backend->is_bad()) {
        std::cerr << "error: backend entered a bad state after: " << action << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "initializing backend..." << std::endl;

    std::unique_ptr<rendering::rendering_backend> backend = 
        std::make_unique<rendering::drm_kms::backend>();

    if (backend->is_bad()) {
        std::cerr << "error: failed to initialize backend" << std::endl;
        return 1;
    }

    auto modes = backend->get_modes();
    if (!check_backend_state(backend, "querying modes")) return 1;

    if (modes.empty()) {
        std::cerr << "error: no available modes" << std::endl;
        return 1;
    }

    std::cout << "setting mode..." << std::endl;
    backend->set_mode(std::move(modes[0]));
    if (!check_backend_state(backend, "setting mode")) return 1;

    std::cout << "loading textures..." << std::endl;
    auto tex_mgr = graphics::texture_manager::load();

	rendering::renderer_2d r2d;
	r2d.set_target(backend.get());
	r2d.set_texture_manager(&tex_mgr);
	const graphics::texture& atlas = tex_mgr.flat_tx_by_id(0);
	r2d.set_font_texture(&atlas);

	std::cout << "Starting Star Wars crawl..." << std::endl;

    std::vector<std::string> crawl_text = {
        "Hello, world! In",
        "a galaxy far, far",
        "away, there was",
        "the kingdom of",
        "Something Regnum",
        "Mundi... And it",
        "was under attack!",
		"Fortunately for",
		"them, they summoned",
		"a brave hero...",
		"you! You were",
		"just an ordinary",
		"japanese high",
		"school student",
		"when you were hit",
		"by truck-kun and",
		"sent to another",
		"world! Now you",
		"have to defeat",
		"the demon lord",
		"and the king will",
		"generously reward",
		"you!"
    };

    int screen_w = backend->get_width();
    int screen_h = backend->get_height();

    int horizon_y = screen_h / 4;
    int base_char_w = 32;
    int base_char_h = 48;
    int line_spacing = 60;

    for(unsigned int t = 0; t <= 1250; ++t) {
        memset(reinterpret_cast<void *>(const_cast<std::uint32_t *>(backend->get_mmio())), 0x00, screen_h * backend->get_pitch());

        float scroll_offset = t * 1.5f;

        for(size_t i = 0; i < crawl_text.size(); ++i) {
            float flat_y = screen_h - scroll_offset + (i * line_spacing);

            if (flat_y <= horizon_y || flat_y > screen_h + line_spacing) {
                continue;
            }

            float scale = (flat_y - horizon_y) / (float)(screen_h - horizon_y);

            int char_w = static_cast<int>(base_char_w * scale);
            int char_h = static_cast<int>(base_char_h * scale);

            if (char_w == 0 || char_h == 0) continue;

            int text_pixel_width = crawl_text[i].length() * char_w;
            int draw_x = (screen_w - text_pixel_width) / 2;
            int draw_y = static_cast<int>(flat_y);

            r2d.draw_text(crawl_text[i].c_str(), draw_x, draw_y, char_w, char_h);
        }

        backend->flush();
        if (!check_backend_state(backend, "flushing frame")) return 1;

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        backend->wait_for_vsync();
        if (!check_backend_state(backend, "waiting for vsync")) return 1;
    }
	std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "exiting gracefully\nresources will be cleaned up automatically" << std::endl;
    return 0;
}
