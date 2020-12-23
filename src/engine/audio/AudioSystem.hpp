#ifndef ME_AUDIO_SYSTEM_HPP
  #define ME_AUDIO_SYSTEM_HPP

#include "../Module.hpp"

namespace me {

  class AudioSystem : public Module {

  protected:

  public:

    explicit AudioSystem(const string &name)
      : Module(MODULE_AUDIO_TYPE, name)
    {
    }

  };

}

#endif
