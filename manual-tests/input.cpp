#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <iomanip>

#include "input/input-backend.h"
#include "input/evdev-backend.h"

bool check_input_state(const std::unique_ptr<input::input_backend>& backend, const std::string& action) {
    if (backend->is_bad()) {
        std::cerr << "error: input backend entered a bad state during: " << action << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "initializing..." << std::endl;

    std::unique_ptr<input::input_backend> backend = std::make_unique<input::evdev::backend>();

    if (backend->is_bad()) {
        std::cerr << "error: failed to initialize evdev backend" << std::endl;
        std::cerr << "(check if you have permissions to read /dev/input/event*)" << std::endl;
        return 1;
    }

    std::cout << "initialized. running..." << std::endl;

    bool running = true;
    input::mouse_state last_mouse = {0, 0, false, false, false};

    while (running) {
        backend->update();
        if (!check_input_state(backend, "polling update")) break;

        if (backend->is_key_down(input::key::esc)) {
            std::cout << "\nexiting..." << std::endl;
            running = false;
        }

        input::mouse_state current_mouse = backend->get_mouse_state();
        
        bool mouse_moved = (current_mouse.x != last_mouse.x || current_mouse.y != last_mouse.y);
        bool clicks_changed = (current_mouse.left != last_mouse.left || 
                               current_mouse.right != last_mouse.right || 
                               current_mouse.middle != last_mouse.middle);

        if (mouse_moved || clicks_changed) {
            std::cout << "\rmouse: POS(" << std::setw(4) << current_mouse.x << "," 
                      << std::setw(4) << current_mouse.y << ") | BTNS: "
                      << (current_mouse.left ? "[L]" : "[ ]")
                      << (current_mouse.middle ? "[M]" : "[ ]")
                      << (current_mouse.right ? "[R]" : "[ ]") 
                      << "                                 "
					  << std::flush;
            
            last_mouse = current_mouse;
        }

		if (backend->is_key_down(input::key::a)) {
            std::cout << "\r[KEY HELD] 'A'                                       " << std::flush;
        }

        if (backend->is_key_down(input::key::l_shift)) {
            std::cout << "\r[KEY HELD] Shift                                     " << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "exiting gracefully\nresources will be cleaned up automatically" << std::endl;
    return 0;
}
