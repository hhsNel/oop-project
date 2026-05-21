#pragma once

#include "math/vec2.h"

namespace combat {
	namespace weapons {
		// Abstrakcyjna strategia strzelania — definiuje co sie dzieje
		// gdy bron zostaje wystrzelona (np. raycast, zapis testowy, fizyczny pocisk).
		class firing_mode {
		public:
			virtual void spawn_bullet(math::vec2 pos, float angle, float damage) = 0;

			// Wywoływane co klatke przez weapon::update(); nadpisz dla pociskow z fizyka.
			virtual void update(float /*dt*/) {}

			virtual ~firing_mode() = default;
		};
	}
}
