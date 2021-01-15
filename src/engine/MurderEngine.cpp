#include "MurderEngine.hpp"

#include "EngineBus.hpp"
#include "Module.hpp"
#include "renderer/Renderer.hpp"
#include "surface/Surface.hpp"
#include "audio/AudioSystem.hpp"

/* class MurderEngine */
me::MurderEngine::MurderEngine(const EngineInfo &engine_info, const EngineBus &engine_bus)
  : engine_info(engine_info), engine_bus(engine_bus), logger("Engine")
{
}

int me::MurderEngine::initialize(int argc, char** argv)
{
  running = true;
#ifndef NDEBUG
  logger.trace(Logger::DEBUG, true);
#endif

  logger.info("running %s engine version [%u.%u.%u]", ME_ENGINE_NAME,
      ME_ENGINE_VERSION_MAJOR, ME_ENGINE_VERSION_MINOR, ME_ENGINE_VERSION_PATCH);

  /* initialize all modules */
  Semaphore init_semaphore;
  init_modules(init_semaphore);
  translate_semaphore(init_semaphore);

  /* main loop */
  while (running)
  {
    /* tick all modules */
    Semaphore tick_semaphore;
    tick_modules(tick_semaphore);
    translate_semaphore(tick_semaphore);
  }
  return 0;
}

int me::MurderEngine::terminate()
{
  running = false;
  printf("terminating...\n");

  /* terminate all modules */
  Semaphore terminate_semaphore;
  terminate_modules(terminate_semaphore);
  translate_semaphore(terminate_semaphore);

  abort();
  return 0;
}

int me::MurderEngine::translate_semaphore(const Semaphore &semaphore)
{
  /* terminate */
  if (semaphore.flags & MODULE_SEMAPHORE_TERMINATE_FLAG)
  {
    logger.debug("received '%s' flag", module_semaphore_flag_name(MODULE_SEMAPHORE_TERMINATE_FLAG));
    if (running)
      terminate();
  }

  /* notify */
  if (semaphore.flags & MODULE_SEMAPHORE_NOTIFY_FLAG)
  {
    engine_bus.get_active_surface_module()->notify();
  }
  return 0;
}

int me::MurderEngine::init_modules(Semaphore &semaphore)
{
  for (Module* module : engine_bus)
  {
    try {
      module->initialize({&semaphore, &engine_bus, &engine_info});
      logger.debug("loaded module %s '%s'", module_type_name(module->get_type()), module->get_name().c_str());
    }catch(const exception &e)
    {
      logger.err("failed to initialize module '%s'\n\t%s", module->get_name().c_str(), e.get_message());
      terminate();
    }
  }
  return 0;
}

int me::MurderEngine::tick_modules(Semaphore &semaphore)
{
  for (Module* module : engine_bus)
  {
    if (module->module_state != MODULE_ACTIVE_STATE)
      continue;

    try {
      module->tick({&semaphore, &engine_bus, &engine_info});
    }catch(const exception &e)
    {
      logger.err("received an error from module '%s'\n\t%s", module->get_name().c_str(), e.get_message());
      terminate();
    }
  }
  return 0;
}

int me::MurderEngine::terminate_modules(Semaphore &semaphore)
{
  for (Module* module : engine_bus)
  {
    try {
      module->terminate({&semaphore, &engine_bus, &engine_info});
    }catch(const exception &e)
    {
      logger.err("failed to terminate module '%s'\n\t%s", module->get_name().c_str(), e.get_message());
      terminate();
    }
  }
  return 0;
}
/* end class MurderEngine */

/* class EngineBus */
me::Module** me::EngineBus::begin() const
{
  return modules;
}

me::Module** me::EngineBus::end() const
{
  return modules + module_count;
}

me::Module* me::EngineBus::get_module(const string &name) const
{
  for (uint32_t i = 0; i < module_count; i++)
  {
    if (name == modules[i]->get_name())
      return modules[i];
  }

  throw exception("no module found with the name '%s'", name.c_str());
}

me::Module* me::EngineBus::get_module(const uint32_t module_type) const
{
  for (uint32_t i = 0; i < module_count; i++)
  {
    if (module_type == modules[i]->get_type())
      return modules[i];
  }

  throw exception("no module found with the type '%s'", module_type_name((ModuleTypes) module_type));
}

me::Surface* me::EngineBus::get_active_surface_module() const
{
  return (Surface*) get_module(MODULE_SURFACE_TYPE);
}

me::Renderer* me::EngineBus::get_active_renderer_module() const
{
  return (Renderer*) get_module(MODULE_RENDERER_TYPE);
}

me::AudioSystem* me::EngineBus::get_active_audio_module() const
{
  return (AudioSystem*) get_module(MODULE_AUDIO_TYPE);
}
/* end class EngineBus */
