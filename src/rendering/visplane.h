#pragma once
#define VISPLANE_H

#include <cstdint>
#include <vector>

#include "assets/asset-manager.h"
#include "assets/ids.h"

namespace rendering {
    // Forward-declared so visplane doesn't need to pull in the full renderer header
    struct frame_rendering_data;

    class visplane {
        /* TODO */
    public:
        float height;
        assets::texture_id tex_id;
        std::uint8_t light_level;

        int min_x, max_x;
        std::vector<int> top, bottom;

        visplane(int const x, unsigned int const sw, float const h, assets::texture_id const tid, std::uint8_t const light);

        // Draws this flat (floor/ceiling) onto the framebuffer described by frd.
        void render(assets::asset_manager const& tex_manager, frame_rendering_data const& frd) const;
    };
}