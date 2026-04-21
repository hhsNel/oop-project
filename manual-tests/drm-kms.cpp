#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <cstdint>

#include "rendering/drm-kms-backend.h"

bool check_backend_state(const std::unique_ptr<rendering::rendering_backend>& backend, const std::string& action) {
    if (backend->is_bad()) {
        std::cerr << "Error: Backend entered a bad state after: " << action << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "Initializing DRM KMS rendering backend..." << std::endl;

    std::unique_ptr<rendering::rendering_backend> backend = 
        std::make_unique<rendering::drm_kms::backend>();

    if (backend->is_bad()) {
        std::cerr << "Error: Failed to initialize DRM KMS backend." << std::endl;
        return 1;
    }

    auto modes = backend->get_modes();
    if (!check_backend_state(backend, "querying modes")) return 1;

    if (modes.empty()) {
        std::cerr << "Error: No rendering modes available on this connector." << std::endl;
        return 1;
    }

    std::cout << "Setting display mode..." << std::endl;
    backend->set_mode(std::move(modes[0]));
    if (!check_backend_state(backend, "setting display mode")) return 1;

    unsigned int width = backend->get_width();
    unsigned int height = backend->get_height();
    unsigned int pitch = backend->get_pitch();
    std::uint32_t* mmio = backend->get_mmio();

    if (!check_backend_state(backend, "fetching frame properties")) return 1;

    if (!mmio) {
        std::cerr << "Error: Memory mapped I/O pointer is null." << std::endl;
        return 1;
    }

    std::cout << "Rendering at " << width << "x" << height 
              << " (pitch: " << pitch << " bytes)" << std::endl;

    unsigned int pixels_per_row = pitch / sizeof(std::uint32_t);

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            std::uint8_t r = (x * 255) / width;
            std::uint8_t g = (y * 255) / height;
            std::uint8_t b = 128;

            std::uint32_t pixel_color = (r << 16) | (g << 8) | b;
            mmio[y * pixels_per_row + x] = pixel_color;
        }
    }

    std::cout << "Flushing frame to screen..." << std::endl;
    backend->flush();
    if (!check_backend_state(backend, "flushing frame")) return 1;

    backend->wait_for_vsync();
    if (!check_backend_state(backend, "waiting for vsync")) return 1;

    std::cout << "Frame rendered. Waiting for 5 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Exiting gracefully. Resources will be cleaned up automatically." << std::endl;
    
    return 0;
}
