#pragma once

// Zbiorczy naglowek wszystkich konkretnych typow broni i trybow strzelania.
// Uzywany TYLKO tam gdzie tworzone sa instancje broni
// (np. level loader, fabryka, testy) — nigdy w kodzie ktory
// jedynie "trzyma" bron (player, monster, pickup).

// Tryby strzelania
#include "recorded-firing-mode.h"
#include "hitscan-firing-mode.h"

// Bronie
#include "pistol.h"
#include "smg.h"
#include "rifle.h"
#include "sniper-rifle.h"
#include "plasma-gun.h"
#include "shotgun.h"
#include "katana.h"
