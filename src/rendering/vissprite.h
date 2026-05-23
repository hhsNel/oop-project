#pragma once
#define VISSPRITE_H

#include <cstdint>
#include <vector>

#include "assets/asset-manager.h"
#include "sprite.h"

namespace rendering {
    struct frame_rendering_data;

    class vissprite {
        float depth;
        int cx1, cx2;
        float proj_x, scale;
        float z_pos;
        assets::texture_id tex_id;
        std::uint8_t light_level;

        std::vector<int> upper_clip;
        std::vector<int> lower_clip;

    public:

        vissprite(sprite const& sprite, float const z, int const clamped_x1, int const clamped_x2, float const px, float const sc, std::uint8_t const light, std::vector<int> const& uc, std::vector<int> const& lc);

		static void pa_sort(std::vector<vissprite> &vec);

        void render(assets::asset_manager const& tex_manager, frame_rendering_data const& frd) const;
    };
}
