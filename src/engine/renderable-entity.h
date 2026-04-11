#pragma once
#define RENDERABLE_ENTITY_H

#include "entity.h"
#include "rendering/renderable.h"

namespace engine {
	class renderable_entity : virtual public entity, public rendering::renderable {
	};
}

