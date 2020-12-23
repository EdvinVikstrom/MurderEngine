#include "MurderEngine.hpp"

#include "EngineBus.hpp"
#include "Module.hpp"

/* class MurderEngine */
me::MurderEngine::MurderEngine(const EngineInfo &engine_info)
  : engine_info(engine_info), engine_bus(engine_info.module_count, engine_info.modules), logger("Engine")
{
}

int me::MurderEngine::initialize(int argc, char** argv)
{
  logger.trace_all();
  running = true;

  Semaphore init_semaphore;
  init_modules(init_semaphore);

  while (running)
  {
    Semaphore tick_semaphore;
    tick_modules(tick_semaphore);
  }
  return 0;
}

int me::MurderEngine::terminate()
{
  running = false;
  printf("terminating...\n");

  Semaphore semaphore;
  terminate_modules(semaphore);

  abort();
  return 0;
}

int me::MurderEngine::translate_semaphore(const Semaphore &semaphore)
{
  if (semaphore.flags & MODULE_SEMAPHORE_TERMINATE_FLAG)
    terminate();
  return 0;
}

int me::MurderEngine::init_modules(Semaphore &semaphore)
{
  for (Module* module : engine_bus)
  {
    try {
      module->initialize({&semaphore, &engine_bus, &engine_info});
      logger.debug("loaded module %s '%s'", module_type_name(module->get_type()), module->get_name().c_str());
      translate_semaphore(semaphore);
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
    try {
      module->tick({&semaphore, &engine_bus, &engine_info});
      translate_semaphore(semaphore);
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
      translate_semaphore(semaphore);
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
me::EngineBus::EngineBus(const uint32_t module_count, class Module** modules)
  : module_count(module_count), modules(modules)
{
}

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

me::Renderer* me::EngineBus::get_active_renderer_module() const
{
  return (Renderer*) get_module(MODULE_RENDERER_TYPE);
}
/* end class EngineBus */
