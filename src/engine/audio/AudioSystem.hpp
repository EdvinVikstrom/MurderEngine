#ifndef ME_AUDIO_SYSTEM_HPP
  #define ME_AUDIO_SYSTEM_HPP

#include "../Module.hpp"

namespace me {

  enum AudioFormat {
    AUDIO_PCM_FORMAT
  };


  struct AudioInfo {
    AudioFormat format;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t depth;
  };

  class AudioTrack {

  public:

    const AudioInfo info;
    const size_t length;
    const float* data;
    mutable size_t position;

    AudioTrack(const AudioInfo &info, const size_t length, const float* data, size_t position)
      : info(info), length(length), data(data)
    {
      this->position = position;
    }

  };

  class AudioSystemModule : public Module {

  protected:

  public:

    explicit AudioSystemModule(const string &name)
      : Module(MODULE_AUDIO_TYPE, name)
    {
    }

    virtual int push(const AudioTrack* track) = 0;

  };

}

#endif
