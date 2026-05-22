// manual-tests/test-launcher.cpp
// Menu-driven launcher for manual tests.
//
// Main menu (menu id 0):
//   Start   (fn 1) → test selection sub-menu
//   Options (fn 2) → options sub-menu (placeholder)
//   Quit    (fn 0) → exit
//
// Test selection (menu id 2):
//   Player HUD 2D        (fn 1) → runs player-hud-2d.out
//   Player Movement 3D   (fn 2) → runs player-movement-3d.out
//   Renderer 2D          (fn 3) → runs renderer-2d.out
//   Software Renderer 4  (fn 4) → runs software-renderer-4.out
//   FPS Demo             (fn 5) → runs fps-demo.out
//   Back to Menu          (fn 0) → returns to main menu
//
// Options (menu id 1):
//   Back (fn 0) → returns to main menu

#include <iostream>
#include <memory>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

#include "rendering/drm-kms/backend.h"
#include "rendering/renderer-2d.h"
#include "assets/asset-manager.h"
#include "util/resource-loader.h"
#include "input/input-backend.h"
#include "input/evdev-backend.h"

enum menu_ids : assets::menu_id {
    MENU_MAIN        = 0,
    MENU_OPTIONS     = 1,
    MENU_TEST_SELECT = 2
};

struct hw_context {
    std::unique_ptr<rendering::drm_kms::backend> r_back;
    std::unique_ptr<input::evdev::backend>        i_back;

    bool init() {
        r_back = std::make_unique<rendering::drm_kms::backend>();
        if (r_back->bad()) { std::cerr << "error: DRM/KMS init failed\n"; return false; }

        auto modes = (*r_back)("modes"_f);
        if (modes.empty()) { std::cerr << "error: no display modes\n"; return false; }
        r_back->push_mode(std::move(modes[0]));
        if (r_back->bad()) { std::cerr << "error: set_mode failed\n"; return false; }

        i_back = std::make_unique<input::evdev::backend>();
        if (i_back->is_bad()) { std::cerr << "error: input init failed\n"; return false; }

        return true;
    }

    void release() {
        i_back.reset();
        r_back.reset();
    }
};

static void run_test(hw_context& hw, std::string const& name) {
    // Zwolnij DRM i input zeby test mogl je przejac
    hw.release();

    // Daj kernelowi czas na zwolnienie DRM/evdev
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    auto test_dir = std::filesystem::read_symlink("/proc/self/exe").parent_path();
    std::string cmd = (test_dir / (name + ".out")).string() + " 2>/tmp/test-launcher.log";
    std::cout << "launching: " << cmd << std::endl;
    int rc = std::system(cmd.c_str());
    std::cout << "test exited with code " << rc << std::endl;

    // Daj czas na pelne zamkniecie testu
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Odzyskaj zasoby po powrocie z testu
    if (!hw.init()) {
        std::cerr << "error: failed to re-initialize after test\n";
    }
}

int main() {
    hw_context hw;
    if (!hw.init()) return 1;

    util::resource_loader rl;
    auto assets = assets::asset_manager::load(rl);

    bool running = true;
    while (running) {
        rendering::renderer_2d r2d(*hw.r_back, assets, assets.flat_tx_by_id(0));
        int result = assets.display_menu(MENU_MAIN, r2d, *hw.r_back, *hw.i_back);

        switch (result) {
            case 0:  // Quit
            case -1: // ESC
                running = false;
                break;

            case 1: { // Start → test selection
                bool selecting = true;
                while (selecting) {
                    rendering::renderer_2d r2d_sel(*hw.r_back, assets, assets.flat_tx_by_id(0));
                    int test = assets.display_menu(MENU_TEST_SELECT, r2d_sel, *hw.r_back, *hw.i_back);
                    switch (test) {
                        case 1: run_test(hw, "player-hud-2d");         break;
                        case 2: run_test(hw, "player-movement-3d");    break;
                        case 3: run_test(hw, "renderer-2d");           break;
                        case 4: run_test(hw, "software-renderer-4");   break;
                        case 5: run_test(hw, "fps-demo");              break;
                        case 0:  // Back
                        case -1: // ESC
                            selecting = false;
                            break;
                        default:
                            break;
                    }
                }
                break;
            }

            case 2: { // Options
                rendering::renderer_2d r2d_opt(*hw.r_back, assets, assets.flat_tx_by_id(0));
                assets.display_menu(MENU_OPTIONS, r2d_opt, *hw.r_back, *hw.i_back);
                break;
            }

            default:
                break;
        }
    }

    std::cout << "exiting test launcher" << std::endl;
    return 0;
}
