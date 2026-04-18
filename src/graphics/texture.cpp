#include "texture.h"

namespace graphics {

    texture::texture(std::vector<std::uint32_t> const &t, std::uint32_t const w, std::uint32_t const h)
        : pixels(t), width(w), height(h) {}

    texture const texture::load_from_bin(util::resource const res) {
        if (res.size < 8) {
            return texture({}, 0, 0);
        }

        const std::uint32_t* data = reinterpret_cast<const std::uint32_t*>(res.begin);
		std::uint32_t w = data[0];
		std::uint32_t h = data[1];

        std::vector<std::uint32_t> pixels(data + 2, data + 2 + (w * h));

        return texture(pixels, w, h);
    }

    unsigned int texture::get_offset(math::fxpt const stretched_len, math::fxpt const stretched_distance) const {
		math::fxpt ratio = math::fxpt(width) / stretched_len;
        unsigned int offset = (float)(stretched_distance * ratio);

        if (offset >= width) offset = width - 1;

        return offset;
    }
}

