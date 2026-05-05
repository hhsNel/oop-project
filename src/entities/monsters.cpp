#include "entities/monsters.h"
#include <cmath>
#include <algorithm>

namespace engine::entities {

// ── monster_basic ────────────────────────────────────────────────────────────
// Idzie prosto w gracza i atakuje melee.
void monster_basic::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dist > attack_range)
        move_toward_target(movement_speed, dt);

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
    }
}

// ── monster_ranged ───────────────────────────────────────────────────────────
// Trzyma preferowany dystans, strzela gdy jest w zasięgu.
void monster_ranged::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dist < preferred_dist - 1.0f)
        move_away_from_target(movement_speed, dt);
    else if (dist > preferred_dist + 1.0f)
        move_toward_target(movement_speed, dt);

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage); // ranged damage na celu
        attack_cooldown = attack_cd_max;
    }
}

// ── monster_assault ──────────────────────────────────────────────────────────
// Burst fire + strafe lewo/prawo, lekki krok naprzód/wstecz między burstami.
void monster_assault::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    // Strafe co strafe_timer sekund
    strafe_timer -= dt;
    if (strafe_timer <= 0.0f) {
        strafe_sign   = -strafe_sign;
        strafe_timer  = 1.2f;
    }
    strafe(movement_speed * 0.7f, strafe_sign, dt);

    // Reposition lekko naprzód/wstecz
    float preferred = attack_range * 0.75f;
    if (dist > preferred + 1.5f) move_toward_target(movement_speed * 0.5f, dt);
    else if (dist < preferred - 1.5f) move_away_from_target(movement_speed * 0.5f, dt);

    // Burst cooldown
    if (burst_cooldown > 0.0f) { burst_cooldown -= dt; return; }

    // Burst aktywny
    if (burst_remaining > 0) {
        burst_timer -= dt;
        if (burst_timer <= 0.0f && dist <= attack_range) {
            melee_attack(attack_damage);
            --burst_remaining;
            burst_timer = burst_interval;
        }
        if (burst_remaining == 0)
            burst_cooldown = 2.5f;
    } else {
        // Start nowego burstu
        burst_remaining = burst_size;
        burst_timer     = 0.0f;
    }
}

// ── monster_sniper ───────────────────────────────────────────────────────────
// Stoi w miejscu, celuje przez shoot_interval sekund, potem strzela.
void monster_sniper::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    aim_timer += dt;
    if (aim_timer >= shoot_interval && dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
        aim_timer       = 0.0f;
    }
}

// ── monster_trapper ──────────────────────────────────────────────────────────
// Po wykryciu gracza szybko się porusza i co jakiś czas "rozstawia pułapkę"
// (tu: task_timer odlicza; faktyczne tworzenie encji pułapki należy do world).
void monster_trapper::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    // Błądzi szybko, co jakiś czas zmienia kierunek
    wander_timer -= dt;
    if (wander_timer <= 0.0f) {
        wander_sign  = -wander_sign;
        wander_timer = 1.5f;
    }
    strafe(movement_speed, wander_sign, dt);
    move_toward_target(movement_speed * 0.3f, dt);

    // Rozstawia pułapkę
    if (traps_placed < max_traps) {
        trap_timer -= dt;
        if (trap_timer <= 0.0f) {
            ++traps_placed;
            trap_timer = trap_interval;
            // Sygnał dla world o stworzeniu pułapki w pozycji position.
            // Implementacja spawnu należy do warstwy world/game-loop.
        }
    }
}

// ── monster_Duzy_Gruby ───────────────────────────────────────────────────────
// Wolno idzie w stronę gracza, mocno uderza.
void monster_Duzy_Gruby::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dist > attack_range)
        move_toward_target(movement_speed, dt);

    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
    }
}

// ── monster_Maly_Szybki ──────────────────────────────────────────────────────
// Odczekuje chwilę, po czym błyskawicznie dashuje w gracza i atakuje.
void monster_Maly_Szybki::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (dash_cooldown > 0.0f) { dash_cooldown -= dt; return; }

    if (is_dashing) {
        move_toward_target(movement_speed * 3.5f, dt);
        dash_timer -= dt;

        if (dist <= attack_range) {
            if (attack_cooldown <= 0.0f) {
                melee_attack(attack_damage);
                attack_cooldown = attack_cd_max;
            }
            is_dashing   = false;
            dash_cooldown = 2.5f;
        }
        if (dash_timer <= 0.0f) {
            is_dashing   = false;
            dash_cooldown = 2.5f;
        }
    } else {
        // Krótka pauza przed dashiem, potem ruszamy
        if (dist <= detection_radius) {
            is_dashing = true;
            dash_timer = 1.0f;
        }
    }
}

// ── monster_all_rounder ──────────────────────────────────────────────────────
// Dystansowy dopóki gracz daleko; zmienia na melee gdy gracz podejdzie blisko.
void monster_all_rounder::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    melee_mode = (dist < melee_threshold);

    if (melee_mode) {
        if (dist > attack_range)
            move_toward_target(movement_speed, dt);
        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage * 1.5f);
            attack_cooldown = attack_cd_max;
        }
    } else {
        float preferred = attack_range * 0.7f;
        if (dist < preferred - 1.0f) move_away_from_target(movement_speed, dt);
        else if (dist > preferred + 1.0f) move_toward_target(movement_speed, dt);

        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage);
            attack_cooldown = attack_cd_max;
        }
    }
}

// ── monster_spawner ──────────────────────────────────────────────────────────
// Stoi w miejscu i co spawn_interval sekund sygnalizuje spawn nowego potwora.
// Faktyczne tworzenie encji należy do warstwy world/game-loop.
void monster_spawner::update(float dt) {
    monster::update(dt);
    if (current_spawns >= max_spawns) return;

    spawn_timer -= dt;
    if (spawn_timer <= 0.0f) {
        ++current_spawns;
        spawn_timer = spawn_interval;
        // Sygnał dla world: stwórz nowego potwora w okolicach position.
    }
}

// ── monster_magic ────────────────────────────────────────────────────────────
// Ładuje mocny atak przez charge_time, po strzale robi kilka kroków w bok
// i odczekuje post_fire_cooldown przed następnym ładowaniem.
void monster_magic::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    // Cooldown po strzale — idzie w bok
    if (post_fire_cooldown > 0.0f) {
        post_fire_cooldown -= dt;
        strafe(movement_speed, sidestep_sign, dt);
        return;
    }

    // Ładowanie
    if (!is_charging) {
        is_charging = true;
        charge_timer = 0.0f;
    }

    sidestep_timer -= dt;
    if (sidestep_timer <= 0.0f) {
        sidestep_sign  = -sidestep_sign;
        sidestep_timer = 0.8f;
    }
    strafe(movement_speed * 0.4f, sidestep_sign, dt);

    charge_timer += dt;
    if (charge_timer >= charge_time && dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown    = attack_cd_max;
        is_charging        = false;
        charge_timer       = 0.0f;
        post_fire_cooldown = 3.0f;
    }
}

// ── monster_elite_tank ───────────────────────────────────────────────────────
// Jak all_rounder + okazjonalny ciężki strzał.
void monster_elite_tank::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    heavy_timer -= dt;
    melee_mode = (dist < melee_threshold);

    if (melee_mode) {
        if (dist > attack_range) move_toward_target(movement_speed, dt);
        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            melee_attack(attack_damage * 1.5f);
            attack_cooldown = attack_cd_max;
        }
    } else {
        float preferred = attack_range * 0.7f;
        if (dist < preferred - 1.0f) move_away_from_target(movement_speed, dt);
        else if (dist > preferred + 1.0f) move_toward_target(movement_speed, dt);

        if (dist <= attack_range && attack_cooldown <= 0.0f) {
            float dmg = (heavy_timer <= 0.0f) ? attack_damage * 3.0f : attack_damage;
            melee_attack(dmg);
            attack_cooldown = attack_cd_max;
            if (heavy_timer <= 0.0f) heavy_timer = heavy_cd;
        }
    }
}

// ── monster_elite_swift ──────────────────────────────────────────────────────
// Krąży wokół gracza strzelając; co jakiś czas szarżuje, potem wraca do kręcenia.
void monster_elite_swift::update(float dt) {
    monster::update(dt);
    if (!has_target()) return;

    float dist = dist_to_target();
    if (dist > detection_radius) return;

    if (charge_cd > 0.0f) charge_cd -= dt;

    if (is_charging) {
        move_toward_target(charge_speed, dt);
        charge_timer -= dt;

        if (dist <= attack_range) {
            if (attack_cooldown <= 0.0f) {
                melee_attack(attack_damage * 2.0f);
                attack_cooldown = attack_cd_max;
            }
            is_charging = false;
            charge_cd   = 5.0f;
        }
        if (charge_timer <= 0.0f) {
            is_charging = false;
            charge_cd   = 5.0f;
        }
        return;
    }

    // Kręci się wokół gracza
    circle_angle += movement_speed * dt;
    auto t = target_ptr.lock();
    if (t) {
        position.x = t->position.x + std::cos(circle_angle) * circle_radius;
        position.y = t->position.y + std::sin(circle_angle) * circle_radius;
    }

    // Strzela będąc w zasięgu
    if (dist <= attack_range && attack_cooldown <= 0.0f) {
        melee_attack(attack_damage);
        attack_cooldown = attack_cd_max;
    }

    // Inicjuje szarżę gdy cooldown minął
    if (charge_cd <= 0.0f) {
        is_charging  = true;
        charge_timer = 0.8f;
    }
}

} // namespace engine::entities
