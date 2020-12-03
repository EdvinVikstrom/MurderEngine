#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"

#include <vector>
#include <string>

namespace me {

  class Surface : public Module {

  protected:

    std::vector<const char*> extensions;

  public:

    explicit Surface(const MurderEngine* engine, const std::string &name)
      : Module(engine, Module::SURFACE, name)
    {
    }

    const std::vector<const char*>& get_extensions() const
    { return extensions; }

  };

}

#endif
