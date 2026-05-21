#include <iostream>
#include <memory>

#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"
#include "assets/texture-manager.h"
#include "util/resource-loader.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"

bool check_backend_state(const std::unique_ptr<rendering::rendering_backend>& backend, const std::string& action) {
    if (backend->bad()) {
        std::cerr << "error: backend entered a bad state after: " << action << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "initializing backend..." << std::endl;

    std::unique_ptr<rendering::rendering_backend> backend =
        std::make_unique<rendering::drm_kms::backend>();

    if (backend->bad()) {
        std::cerr << "error: failed to initialize backend" << std::endl;
        return 1;
    }

    auto modes = (*backend)("modes"_f);
    if (!check_backend_state(backend, "querying modes")) return 1;

    if (modes.empty()) {
        std::cerr << "error: no available modes" << std::endl;
        return 1;
    }

    backend->push_mode(std::move(modes[0]));
    if (!check_backend_state(backend, "setting mode")) return 1;

    std::cout << "loading assets..." << std::endl;
    util::resource_loader rl;
    auto tex_mgr = assets::texture_manager::load(rl);

    rendering::renderer_2d r2d(*backend.get(), tex_mgr, tex_mgr.flat_tx_by_id(0));

    // input backend drives the menu event loop
    std::unique_ptr<input::input_backend> in = std::make_unique<input::evdev::backend>();

    std::cout << "displaying test menu (menu id 0)..." << std::endl;
    std::cout << "click an element or press ESC to exit" << std::endl;

    int result = tex_mgr.display_menu(0, r2d, *backend, in.get());

    if (result == -1) {
        std::cout << "menu cancelled (ESC)" << std::endl;
    } else {
        std::cout << "menu returned function id: " << result << std::endl;
    }

    return 0;
}
