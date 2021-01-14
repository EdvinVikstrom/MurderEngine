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
    MODULE_SEMAPHORE_TERMINATE_FLAG = 1,
    MODULE_SEMAPHORE_NOTIFY_FLAG = 1 << 1
  };

  enum ModuleState {
    MODULE_ACTIVE_STATE,
    MODULE_IDLE_STATE
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

    mutable ModuleState module_state;

  public:

    Module(const ModuleTypes module_type, const string &name, ModuleState module_state = MODULE_ACTIVE_STATE)
      : module_type(module_type), name(name)
    {
      this->module_state = module_state;
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

  static inline const char* module_semaphore_flag_name(const ModuleSemaphoreFlags flag)
  {
    switch (flag)
    {
      case MODULE_SEMAPHORE_TERMINATE_FLAG: return "TERMINATE";
      default: return "";
    }
  }

}

#endif
