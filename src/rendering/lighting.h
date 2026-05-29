#pragma once

#include <cstdint>

namespace rendering {
	class lighting {
		static constexpr float dl_start   = 64.0f;
		static constexpr float dl_density = 0.25f;

	public:

		__attribute__((always_inline)) static inline int calculate(std::uint8_t const sector_light_level, float const depth) {
			if(depth > dl_start) {
				int corrected_light = sector_light_level - (depth - dl_start) * dl_density;
				if(corrected_light < 0) corrected_light = 0;
				return corrected_light;
			} else {
				return sector_light_level;
			}
		}

		__attribute__((always_inline)) static inline std::uint32_t apply(std::uint32_t const orig, int light) {
			std::uint32_t swar_rb = orig & 0x00ff00ff;
			std::uint32_t swar_g  = orig & 0x0000ff00;

			swar_rb = ((swar_rb * light) >> 8) & 0x00ff00ff;
			swar_g  = ((swar_g  * light) >> 8) & 0x0000ff00;

			return swar_rb | swar_g;
		}
	};
}
