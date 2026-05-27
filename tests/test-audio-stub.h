#pragma once
#include "audio/audio-backend.h"
#include "audio/audio-mixer.h"
#include "assets/asset-manager.h"
#include "util/resource-loader.h"

namespace test_helpers {
	class null_audio_backend : public audio::audio_backend {
		audio::audio_format fmt{44100, 2, 16};
	public:
		bool open(audio::audio_format const&) override { return true; }
		void close() override {}
		long write(void const*, unsigned long frames) override { return static_cast<long>(frames); }
		void drain() override {}
		void pause() override {}
		void resume() override {}
		audio::audio_format operator()(util::component_tag<"current_format">) const override { return fmt; }
		bool bad() const override { return false; }
	};

	struct test_audio {
		null_audio_backend backend;
		audio::audio_mixer mixer;
		util::resource_loader rl;
		assets::asset_manager am;

		test_audio()
			: mixer(backend, {44100, 2, 16}),
			  am(assets::asset_manager::load(rl)) {}
	};
}
