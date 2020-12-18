#ifndef ME_PORTAUDIO_HPP
  #define ME_PORTAUDIO_HPP

#include "../AudioSystem.hpp"

#include <portaudio.h>

namespace me {

  class PortAudio : public AudioSystem {

  private:

    Logger* logger;

    PaStream* stream;

  public:

    explicit PortAudio(const MurderEngine* engine);

    int initialize() override;
    int terminate() override;
    int tick(const Context context) override;

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
