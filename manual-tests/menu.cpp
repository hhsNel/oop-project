#include <iostream>
#include <memory>
#include <string>

#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"
#include "assets/asset-manager.h"
#include "util/resource-loader.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"

static std::string replace_token(std::string_view tmpl, std::string_view token, std::string_view value) {
    std::string result(tmpl);
    for (std::size_t pos = 0; (pos = result.find(token, pos)) != std::string::npos;) {
        result.replace(pos, token.size(), value);
        pos += value.size();
    }
    return result;
}

static bool check_backend_state(std::unique_ptr<rendering::rendering_backend> const &backend, std::string const &action) {
    if (backend->bad()) {
        std::cerr << "error: backend entered a bad state after: " << action << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "initializing backend..." << std::endl;

    std::unique_ptr<rendering::rendering_backend> backend = std::make_unique<rendering::drm_kms::backend>();

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
    auto tex_mgr = assets::asset_manager::load(rl);

    rendering::renderer_2d r2d(*backend.get(), tex_mgr, tex_mgr.flat_tx_by_id(0));

    std::unique_ptr<input::input_backend> in = std::make_unique<input::evdev::backend>();

    std::string player_name = "Some runtime player name";
    int         score       = 420;

    assets::menu &main_menu = tex_mgr.menu_by_id(0);

    main_menu.selective_formatter("{name}|{score}", [&](std::string_view tmpl) {
        std::string s = replace_token(tmpl, "{name}",  player_name);
        return          replace_token(s,    "{score}", std::to_string(score));
    });

    std::cout << "displaying main menu..." << std::endl;
    std::cout << "click an element or press ESC to exit" << std::endl;

    int result = main_menu.display(r2d, *backend, in.get());

    if (result == -1) {
        std::cout << "menu cancelled (ESC)" << std::endl;
    } else {
        std::cout << "menu returned function id: " << result << std::endl;
    }

    return 0;
}

