#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>
#include <algorithm>

#include "rendering/drm-kms/backend.h"
#include "graphics/texture-manager.h"
#include "graphics/texture.h"

bool check_backend_state(const std::unique_ptr<rendering::rendering_backend>& backend, const std::string& action) {
    if (backend->is_bad()) {
        std::cerr << "error: backend entered a bad state after: " << action << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
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

	char tx_type = 'w';
	if(argc > 1) tx_type = *argv[1];

	unsigned int id = 0;
	if(argc > 2) id = std::atoi(argv[2]);

    std::cout << "loading textures..." << std::endl;
	util::resource_loader rl;
    auto tex_mgr = graphics::texture_manager::load(rl);
    const graphics::texture *wall_tex;
	switch(tx_type) {
	case 'f':
		wall_tex = &tex_mgr.flat_tx_by_id(id);
		break;
	case 's':
		wall_tex = &tex_mgr.sprite_tx_by_id(id);
		break;
	default:
		wall_tex = &tex_mgr.wall_tx_by_id(id);
		break;
	}
    
    std::cout << "loaded wall texture 0 (" << wall_tex->width << "x" << wall_tex->height << ")" << std::endl;

    unsigned int screen_width = backend->get_width();
    unsigned int screen_height = backend->get_height();
    unsigned int pitch = backend->get_pitch();
    std::uint32_t* mmio = backend->get_mmio();

    if (!check_backend_state(backend, "fetching frame properties")) return 1;

    if (!mmio) {
        std::cerr << "error: MMIO pointer is null" << std::endl;
        return 1;
    }

    std::cout << "rendering at " << screen_width << "x" << screen_height 
              << " (pitch: " << pitch << " bytes)" << std::endl;

    unsigned int pixels_per_row = pitch / sizeof(std::uint32_t);

    for (unsigned int y = 0; y < screen_height; ++y) {
        for (unsigned int x = 0; x < screen_width; ++x) {
            mmio[y * pixels_per_row + x] = 0x222222; 
        }
    }

    unsigned int draw_width = std::min(screen_width, wall_tex->width);
    unsigned int draw_height = std::min(screen_height, wall_tex->height);

	for (unsigned int x = 0; x < draw_width; ++x) {
		for (unsigned int y = 0; y < draw_height; ++y) {
            std::uint32_t pixel_color = wall_tex->pixels[y + x * wall_tex->height];
            
            if (!wall_tex->has_transparency || pixel_color != 0xFFFF00FF) {
                mmio[y * pixels_per_row + x] = pixel_color;
            }
        }
    }

    std::cout << "flushing frame to screen..." << std::endl;
    backend->flush();
    if (!check_backend_state(backend, "flushing frame")) return 1;

    backend->wait_for_vsync();
    if (!check_backend_state(backend, "waiting for vsync")) return 1;

    std::cout << "frame rendered! sleeping for 5 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "exiting gracefully\nresources will be cleaned up automatically" << std::endl;
    return 0;
}
