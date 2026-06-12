#pragma once

namespace engine {
	class entity {
	public:
		/* tick an entity */
		virtual void update(float const dt) = 0;
		/* virtual destructor */
		virtual ~entity() = default;
	};
}
