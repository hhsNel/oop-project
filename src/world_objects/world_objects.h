#pragma once
#define ENGINE_WORLD_OBJECTS_H

#include "engine/entity.h"
#include "util/componentized.h"
#include "math/vec2.h"

// Forward declarations
namespace entities { class player; }
namespace combat { namespace weapons { class weapon; } }

    namespace world_object {

        /*
        Abstract base for all collectible world items.
        Each frame, the game loop checks in_range(player.position) and calls
        on_pickup(player) when the player is close enough.
        */
        class pickup : public engine::entity, public util::componentized<pickup> {
            friend class util::componentized<pickup>;

            math::vec2 position;
            float      pickup_radius;
        protected:
            [[=util::component_field{}]] bool collected;
        public:
            pickup(math::vec2 pos, float radius = 20.0f)
                : position(pos), pickup_radius(radius), collected(false) {}

            void update(float /*dt*/) override {}

            bool in_range(math::vec2 player_pos) const;

            virtual void on_pickup(entities::player& p) = 0;
            virtual ~pickup() = default;
        };

        class health_pickup : public pickup {
            float heal_amount;
        public:
            health_pickup(math::vec2 pos, float amount, float radius = 20.0f)
                : pickup(pos, radius), heal_amount(amount) {}

            void on_pickup(entities::player& p) override;
        };

        class armor_pickup : public pickup {
            float armor_amount;
        public:
            armor_pickup(math::vec2 pos, float amount, float radius = 20.0f)
                : pickup(pos, radius), armor_amount(amount) {}

            void on_pickup(entities::player& p) override;
        };

        // Adds ammo to the player's weapon matching weapon_id.
        class ammo_pickup : public pickup {
            unsigned int weapon_id;
            unsigned int amount;
        public:
            ammo_pickup(math::vec2 pos, unsigned int wid, unsigned int amt, float radius = 20.0f)
                : pickup(pos, radius), weapon_id(wid), amount(amt) {}

            void on_pickup(entities::player& p) override;
        };
        
        class weapon_pickup : public pickup {
            combat::weapons::weapon* provided_weapon;
        public:
            weapon_pickup(math::vec2 pos, combat::weapons::weapon* w, float radius = 25.0f)
                : pickup(pos, radius), provided_weapon(w) {}

            void on_pickup(entities::player& p) override;
        };

    }
