#ifndef ME_AUDIO_SYSTEM_HPP
  #define ME_AUDIO_SYSTEM_HPP

#include "../Module.hpp"

namespace me {

  class AudioSystem : public Module {

  protected:

  public:

    explicit AudioSystem(const MurderEngine* engine, const string &name)
      : Module(engine, Module::AUDIO, name)
    {
    }

  };

}

#endif
