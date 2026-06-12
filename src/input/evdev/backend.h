#pragma once
#include <vector>
#include <bitset>
#include <linux/input.h>
#include "input/input-backend.h"

namespace input {
	namespace evdev {
		class backend : public input::input_backend {
		private:
			/* a single open evdev file */
			struct device_node {
				int fd;
				bool is_keyboard;
				bool is_mouse;
			};

			/* all open files */
			std::vector<device_node> devices;
			/* currently pressed keys */
			std::bitset<KEY_CNT> key_map;
			/* current mouse state */
			mouse_state mouse;

			/* last absolute state for EV_ABS */
			int last_abs_x;
			int last_abs_y;

			/* epoll file descriptor for reading only changed files */
			int epoll_fd;

			/* emulated-to-native map */
			uint16_t map_to_native(key k) const;
			/* probe all evdev devices */
			void probe_devices();
			/* probe a single evdev device */
			void probe_device(std::string const& dev_name);
			/* process a single struct input_event from evdev files */
			void process_event(struct input_event const& ev);

		public:
			/* constructor */
			backend();
			/* destructor */
			~backend() override;

			/* update internal state */
			void update() override;
			/* read key from internal state */
			bool is_key_down(key k) const override;
			/* read internal mouse state */
			mouse_state get_mouse_state() const override;
			/* reset internal mouse state */
			void reset_mouse_state(int nx, int ny) override;
		};
	}
}

