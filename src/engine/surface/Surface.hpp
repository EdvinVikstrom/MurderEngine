#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class Surface : public Module {

  public:

    struct Monitor {
      void* ptr = nullptr;
    };

    struct Config {
      string title;
      Monitor monitor;

      /* for window stuff */
      uint32_t width = -1, height = -1;
    };

    struct Callbacks {
      int (*init_surface) (Config&);
    };

  protected:

    Callbacks callbacks;
    Config config;

  public:

    explicit Surface(const string &name, Callbacks &callbacks)
      : Module(MODULE_SURFACE_TYPE, name)
    {
      this->callbacks.init_surface = callbacks.init_surface;
    }

    virtual int get_size(uint32_t &width, uint32_t &height) const = 0;
    virtual size_t get_current_frame_index() const = 0;

    virtual int refresh() = 0;
    virtual int update_config(Config &new_config) = 0;

  };

}

#endif
