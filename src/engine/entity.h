#pragma once
#define ENTITY_H


namespace engine {
	class entity {
	public:
		virtual void update(float const dt) = 0;
		virtual ~entity() = default;
	};
}

