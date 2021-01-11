#ifndef ME_PORTAUDIO_HPP
  #define ME_PORTAUDIO_HPP

#include "../AudioSystem.hpp"

#include "../../Logger.hpp"

#include <portaudio.h>

namespace me {

  class PortAudio : public AudioSystem {

  private:

    Logger logger;

    PaStream* stream;

  protected:

    const AudioTrack* tracks[4];

  public:

    explicit PortAudio();

    int initialize(const ModuleInfo) override;
    int terminate(const ModuleInfo) override;
    int tick(const ModuleInfo) override;

    int push(const AudioTrack* track) override;

  protected:

    static int pa_stream_callback(
	const void* input,
	void* output,
	uint64_t frame_count,
	const PaStreamCallbackTimeInfo *time_info,
	PaStreamCallbackFlags status_flags,
	void* user_data);

  };

}

#endif
