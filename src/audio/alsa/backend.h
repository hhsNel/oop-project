#pragma once

#include <alsa/asoundlib.h>
#include "audio/audio-backend.h"

namespace audio {
	namespace alsa {
		class backend : public audio::audio_backend {
		private:
			/* error check */
			bool is_bad;

			/* also pcm */
			snd_pcm_t* pcm_handle;
			/* supported format */
			audio_format fmt;
			/* supported is currently paused */
			bool paused;

			/* convert to alsa */
			snd_pcm_format_t to_alsa_format(unsigned int bits) const;

			/* configure hardware */
			bool configure_hw_params();

		public:
			/* constructor */
			backend();
			/* destructor */
			~backend() override;

			/* set a new format */
			bool open(audio_format const& format) override;
			/* stop */
			void close() override;
			/* send pcm data */
			long write(void const* data, unsigned long frames) override;
			/* drain the pcm buffer */
			void drain() override;
			/* pause audio */
			void pause() override;
			/* resume audio */
			void resume() override;
			/* componentized-like api */
			audio_format operator()(util::component_tag<"current_format">) const override;
			/* error checking */
			bool bad() const override;
		};
	}
}
