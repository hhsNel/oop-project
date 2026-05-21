#pragma once

#include "entities/entities.h"

namespace entities {

	// Usunieto componentized — phase_count jest polem wewnetrznym,
	// nie wymaga eksportu na zewnatrz klasy. Brak update() — do implementacji.
	class monster_boss : public monster {
		int phase_count;

	public:
		monster_boss() : monster(1000.0f, 300.0f, 1.5f, 3.0f, 30.0f, 20.0f, 1.0f),
		    phase_count(3) {}
		int phases() const { return phase_count; }
	};

}
