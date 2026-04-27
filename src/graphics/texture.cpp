#include "texture.h"

#include <iostream>

namespace graphics {

    texture::texture(std::vector<std::uint32_t> const &t, std::uint32_t const w, std::uint32_t const h, bool const transparent)
        : pixels(std::move(t)), width(w), height(h), has_transparency(transparent) {}

    texture const texture::load_from_bin(util::resource const res) {
        if (res.size < 12) {
			std::cerr << "empty texture" << std::endl;
			/* causes crash when used, INTENTIONAL */
            return texture({}, 0, 0, false);
        }

        const std::uint32_t* data = reinterpret_cast<const std::uint32_t*>(res.begin);
        std::uint32_t w = data[0];
        std::uint32_t h = data[1];
        bool has_transparency = data[2] != 0;

        const std::uint8_t* byte_data = reinterpret_cast<const std::uint8_t*>(res.begin) + 12;
        
        std::vector<std::uint32_t> pixels;
        pixels.reserve(w * h);

        for (std::uint32_t i = 0; i < w * h; ++i) {
            std::uint32_t r = byte_data[i * 4 + 0];
            std::uint32_t g = byte_data[i * 4 + 1];
            std::uint32_t b = byte_data[i * 4 + 2];
            std::uint32_t a = byte_data[i * 4 + 3];

            std::uint32_t pixel_color = (a << 24) | (r << 16) | (g << 8) | b;
            pixels.push_back(pixel_color);
        }

        return texture(pixels, w, h, has_transparency);
    }
}
