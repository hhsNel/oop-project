#pragma once
#include <vector>
#include <bitset>
#include <linux/input.h>
#include "input-backend.h"

namespace input {
	namespace evdev {
		class backend : public input::input_backend {
		private:
			struct device_node {
				int fd;
				bool is_keyboard;
				bool is_mouse;
			};

			std::vector<device_node> devices;
			std::bitset<KEY_CNT> key_map;
			mouse_state mouse;

			int last_abs_x;
			int last_abs_y;

			uint16_t map_to_native(key k) const;
			void probe_devices();
			void probe_device(std::string const dev_name);
			void process_event(const struct input_event& ev);

		public:
			backend();
			~backend() override;

			void update() override;
			bool is_key_down(key k) const override;
			mouse_state get_mouse_state() const override;
		};
	}
}
