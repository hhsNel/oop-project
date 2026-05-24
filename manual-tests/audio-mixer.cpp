#include <iostream>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>

#include "audio/audio-mixer.h"
#include "audio/alsa/backend.h"
#include "assets/asset-manager.h"
#include "util/resource-loader.h"

int main() {
    std::cout << "initialising...\n";

    util::resource_loader resld;
    assets::asset_manager asset_mgr = assets::asset_manager::load(resld);

    constexpr audio::audio_format FORMAT { 44100, 2, 16 };
    constexpr unsigned long       FRAMES  = 441;  // ~10 ms per step

    audio::alsa::backend backend;
    audio::audio_mixer   mixer(backend, FORMAT);

    if (backend.is_bad()) {
        std::cerr << "error: audio backend failed to initialise\n";
        return 1;
    }

    // Put stdin in non-blocking mode so we can poll it alongside the mixer.
    // O_NONBLOCK on fd 0 means read() returns -1/EAGAIN immediately when
    // there is nothing to read instead of blocking the loop.
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

    std::cout << "assets loaded. running...\n";
    std::cout << "commands: <enter> = play sound 0 | q<enter> = quit\n> " << std::flush;

    bool quitting = false;
    std::string line_buf;

    while (true) {
        // --- Non-blocking stdin read, one character at a time ---
        char ch;
        while (read(STDIN_FILENO, &ch, 1) == 1) {
            if (ch == '\n') {
                if (line_buf == "q" || line_buf == "Q") {
                    std::cout << "waiting for sounds to finish...\n";
                    quitting = true;
                } else {
                    try {
                        mixer.play(asset_mgr.audio_clip_by_id(0));
                        std::cout << "[audio] playing clip 0\n> " << std::flush;
                    } catch (std::out_of_range const&) {
                        std::cerr << "error: clip 0 not found\n> " << std::flush;
                    }
                }
                line_buf.clear();
            } else {
                line_buf += ch;
            }
        }

        // --- Mix + write one ~10 ms chunk (blocks for the chunk duration) ---
        mixer.step(FRAMES);

        if (backend.is_bad()) {
            std::cerr << "error: audio backend entered a bad state\n";
            return 1;
        }

        // --- Exit once all sounds have finished playing ---
        if (quitting && mixer.is_silent()) {
            backend.drain();
            break;
        }
    }

    std::cout << "exiting gracefully\nresources will be cleaned up automatically\n";
    return 0;
}
