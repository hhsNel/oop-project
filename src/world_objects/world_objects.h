#pragma once
#define ENGINE_WORLD_OBJECTS_H

#include "engine/entity.h"
#include "math/vec2.h"

// Forward declarations
namespace entities { class player; }
namespace combat { namespace weapons { class weapon; } }

    namespace world_object {

        // Abstract base for all collectible world items.
        // Each frame, the game loop checks in_range(player.position) and calls
        // on_pickup(player) when the player is close enough.
        class pickup : public engine::entity {
        public:
            math::vec2 position;
            float      pickup_radius;
            bool       collected;

            pickup(math::vec2 pos, float radius = 20.0f)
                : position(pos), pickup_radius(radius), collected(false) {}

            void update(float /*dt*/) override {}

            // Returns true when the player is within pickup_radius.
            bool in_range(math::vec2 player_pos) const;

            // Apply the pickup effect to the player; sets collected = true.
            virtual void on_pickup(entities::player& p) = 0;
            virtual ~pickup() = default;
        };

        // Restores player HP up to their maximum.
        class health_pickup : public pickup {
        public:
            float heal_amount;

            health_pickup(math::vec2 pos, float amount, float radius = 20.0f)
                : pickup(pos, radius), heal_amount(amount) {}

            void on_pickup(entities::player& p) override;
        };

        // Restores player armor/shield up to their maximum.
        class armor_pickup : public pickup {
        public:
            float armor_amount;

            armor_pickup(math::vec2 pos, float amount, float radius = 20.0f)
                : pickup(pos, radius), armor_amount(amount) {}

            void on_pickup(entities::player& p) override;
        };

        // Adds ammo to the player's weapon matching weapon_id.
        class ammo_pickup : public pickup {
        public:
            unsigned int weapon_id;
            unsigned int amount;

            ammo_pickup(math::vec2 pos, unsigned int wid, unsigned int amt, float radius = 20.0f)
                : pickup(pos, radius), weapon_id(wid), amount(amt) {}

            void on_pickup(entities::player& p) override;
        };

        // Adds a pre-existing weapon to the player's loadout.
        // The weapon is owned externally (e.g. by the level/game state).
        class weapon_pickup : public pickup {
        public:
            combat::weapons::weapon* provided_weapon;

            weapon_pickup(math::vec2 pos, combat::weapons::weapon* w, float radius = 25.0f)
                : pickup(pos, radius), provided_weapon(w) {}

            void on_pickup(entities::player& p) override;
        };

    }
