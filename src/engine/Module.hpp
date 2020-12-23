#ifndef ME_MODULE_HPP
  #define ME_MODULE_HPP

#include "util/Semaphore.hpp"
#include "EngineBus.hpp"
#include "EngineInfo.hpp"

#include <lme/string.hpp>

namespace me {

  enum ModuleTypes {
    MODULE_SURFACE_TYPE,
    MODULE_RENDERER_TYPE,
    MODULE_AUDIO_TYPE,
    MODULE_IO_TYPE,
    MODULE_OTHER_TYPE
  };

  enum ModuleSemaphoreFlags {
    MODULE_SEMAPHORE_TERMINATE_FLAG = 1
  };


  struct ModuleInfo {
    mutable Semaphore* semaphore;
    const EngineBus* engine_bus;
    const EngineInfo* engine_info;
  };


  class Module {

  protected:

    friend class MurderEngine;

    const ModuleTypes module_type;
    const string name;

  public:

    Module(const ModuleTypes module_type, const string &name)
      : module_type(module_type), name(name)
    {
    }

    const ModuleTypes get_type() const
    {
      return module_type;
    }

    const string& get_name() const
    {
      return name;
    }


  protected:

    virtual int initialize(const ModuleInfo) = 0;
    virtual int terminate(const ModuleInfo) = 0;
    virtual int tick(const ModuleInfo) = 0;

  };


  static inline const char* module_type_name(const ModuleTypes type)
  {
    switch (type)
    {
      case MODULE_RENDERER_TYPE: return "RENDERER";
      case MODULE_AUDIO_TYPE: return "AUDIO";
      case MODULE_SURFACE_TYPE: return "SURFACE";
      case MODULE_IO_TYPE: return "IO";
      case MODULE_OTHER_TYPE: return "OTHER";
      default: return "";
    }
  }

}

#endif
