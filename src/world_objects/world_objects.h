#pragma once
#define ENGINE_WORLD_OBJECTS_H

#include "engine/entity.h"
#include "util/componentized.h"
#include "math/vec2.h"

// Forward declarations
namespace entities { class player; }
namespace combat { namespace weapons { class weapon; enum class ammo_type : unsigned int; } }

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
            bool collected;
        public:
            pickup(math::vec2 pos, float radius = 20.0f);

            void update(float /*dt*/) override {}

            bool in_range(math::vec2 player_pos) const;

            virtual void on_pickup(entities::player& p) = 0;
            virtual ~pickup() = default;
        };

        class health_pickup : public pickup {
            float heal_amount;
        public:
            health_pickup(math::vec2 pos, float amount, float radius = 20.0f);
            void on_pickup(entities::player& p) override;
        };

        class armor_pickup : public pickup {
            float armor_amount;
        public:
            armor_pickup(math::vec2 pos, float amount, float radius = 20.0f);
            void on_pickup(entities::player& p) override;
        };

        // Adds reserve magazines to the player's weapon matching ammo_type.
        class ammo_pickup : public pickup {
            combat::weapons::ammo_type type;
            int amount;
        public:
            ammo_pickup(math::vec2 pos, combat::weapons::ammo_type type, int amt, float radius = 20.0f);
            void on_pickup(entities::player& p) override;
        };

        class weapon_pickup : public pickup {
            combat::weapons::weapon* provided_weapon;
        public:
            weapon_pickup(math::vec2 pos, combat::weapons::weapon* w, float radius = 25.0f);
            void on_pickup(entities::player& p) override;
        };

    }
