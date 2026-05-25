#include "backend.h"

namespace audio {
	namespace alsa {

		backend::backend() :
			is_bad(false),
			pcm_handle(nullptr),
			fmt{48000, 2, 16},
			paused(false) {}

		backend::~backend() {
			close();
		}

		snd_pcm_format_t backend::to_alsa_format(unsigned int bits) const {
			switch (bits) {
				case 8:  return SND_PCM_FORMAT_U8;
				case 16: return SND_PCM_FORMAT_S16_LE;
				case 24: return SND_PCM_FORMAT_S24_LE;
				case 32: return SND_PCM_FORMAT_S32_LE;
				default: return SND_PCM_FORMAT_S16_LE;
			}
		}

		bool backend::configure_hw_params() {
			snd_pcm_hw_params_t* params;
			snd_pcm_hw_params_alloca(&params);

			if (snd_pcm_hw_params_any(pcm_handle, params) < 0)
				return false;

			if (snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
				return false;

			if (snd_pcm_hw_params_set_format(pcm_handle, params, to_alsa_format(fmt.bits_per_sample)) < 0)
				return false;

			if (snd_pcm_hw_params_set_channels(pcm_handle, params, fmt.channels) < 0)
				return false;

			unsigned int rate = fmt.sample_rate;
			if (snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, nullptr) < 0)
				return false;
			fmt.sample_rate = rate;

			snd_pcm_uframes_t buffer_size = rate / 10;
			snd_pcm_uframes_t period_size = rate / 40;
			snd_pcm_hw_params_set_buffer_size_near(pcm_handle, params, &buffer_size);
			snd_pcm_hw_params_set_period_size_near(pcm_handle, params, &period_size, nullptr);

			if (snd_pcm_hw_params(pcm_handle, params) < 0)
				return false;

			return true;
		}

		bool backend::open(audio_format const& format) {
			close();
			fmt = format;

			int err = snd_pcm_open(&pcm_handle, "pulse", SND_PCM_STREAM_PLAYBACK, 0);
			if (err < 0)
				err = snd_pcm_open(&pcm_handle, "pipewire", SND_PCM_STREAM_PLAYBACK, 0);
			if (err < 0)
				err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
			if (err < 0) {
				is_bad = true;
				pcm_handle = nullptr;
				return false;
			}

			if (!configure_hw_params()) {
				snd_pcm_close(pcm_handle);
				pcm_handle = nullptr;
				is_bad = true;
				return false;
			}

			if (snd_pcm_prepare(pcm_handle) < 0) {
				snd_pcm_close(pcm_handle);
				pcm_handle = nullptr;
				is_bad = true;
				return false;
			}

			is_bad = false;
			paused = false;
			return true;
		}

		void backend::drain() {
			if (pcm_handle && !is_bad)
				snd_pcm_drain(pcm_handle);
		}

		void backend::close() {
			if (pcm_handle) {
				snd_pcm_drain(pcm_handle);
				snd_pcm_close(pcm_handle);
				pcm_handle = nullptr;
			}
			paused = false;
		}

		long backend::write(void const* data, unsigned long frames) {
			if (!pcm_handle || is_bad || paused) return -1;

			snd_pcm_sframes_t written = snd_pcm_writei(
				pcm_handle,
				data,
				static_cast<snd_pcm_uframes_t>(frames)
			);

			if (written == -EPIPE) {
				snd_pcm_prepare(pcm_handle);
				written = snd_pcm_writei(
					pcm_handle,
					data,
					static_cast<snd_pcm_uframes_t>(frames)
				);
			}

			if (written < 0) {
				is_bad = true;
				return -1;
			}

			return static_cast<long>(written);
		}

		void backend::pause() {
			if (!pcm_handle || is_bad) return;

			if (snd_pcm_pause(pcm_handle, 1) < 0)
				snd_pcm_drop(pcm_handle);

			paused = true;
		}

		void backend::resume() {
			if (!pcm_handle || is_bad) return;

			if (snd_pcm_pause(pcm_handle, 0) < 0)
				snd_pcm_prepare(pcm_handle);

			paused = false;
		}

		audio_format backend::operator()(util::component_tag<"current_format">) const {
			return fmt;
		}

		bool backend::bad() const {
			return is_bad;
		}
	}
}
