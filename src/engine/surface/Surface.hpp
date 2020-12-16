#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "../Module.hpp"

#include <vulkan/vulkan.h>

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class Surface : public Module {

  protected:

    vector<const char*> extensions;

  public:

    explicit Surface(const MurderEngine* engine, const string &name)
      : Module(engine, Module::SURFACE, name)
    {
    }

    const vector<const char*>& get_extensions() const
    { return extensions; }

    virtual int get_size(uint32_t &width, uint32_t &height) const = 0;

    virtual int create_surface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;

  };

}

#endif
