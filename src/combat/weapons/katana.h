#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			katana(audio::audio_mixer* mix = nullptr,
			       assets::audio_clip const* fire_snd = nullptr);

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
