#pragma once
#include "weapon.h"

namespace combat {
	namespace weapons {
		class katana : public weapon {
		public:
			katana(audio::audio_mixer& mix,
			       assets::asset_manager const& am);

			bool can_fire() const override;
			void fire(math::vec2 pos, float angle) override;
			void reload() override;
		};
	}
}
