#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class Surface : public Module {

  public:

  protected:

  public:

    explicit Surface(const string &name)
      : Module(MODULE_SURFACE_TYPE, name)
    {
    }

    virtual int get_size(uint32_t &width, uint32_t &height) const = 0;
    virtual size_t get_current_frame_index() const = 0;

  };

}

#endif
