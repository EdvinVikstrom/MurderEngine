#include "MurderEngine.hpp"

#include "Module.hpp"

me::MurderEngine::MurderEngine(const AppConfig &app_config)
  : app_config(app_config), logger("Engine")
{
}

const me::AppConfig& me::MurderEngine::get_app_config() const
{
  return app_config;
}

const me::Logger& me::MurderEngine::get_logger() const
{
  return logger;
}

int me::MurderEngine::initialize(int argc, char** argv)
{
  logger.trace_all();
  running = true;
  return 0;
}

int me::MurderEngine::initialize_loop()
{
  while (running)
  {
    for (Module* module : modules)
    {
      if (!module->is_active())
	continue;

      try {
	module->tick();
      }catch (const exception &e)
      {
	print_module_exception(*module, e);
	return 1;
      }
    }
  }
  return 0;
}

int me::MurderEngine::terminate()
{
  running = false;
  printf("terminating...\n");
  return 0;
}

int me::MurderEngine::load_module(Module* module)
{
  modules.push_back(module);
  module->enable();

  try {
    module->initialize();
  }catch (const exception &e)
  {
    print_module_exception(*module, e);
    return 1;
  }

  logger.debug("loaded %s module '%s'", Module::type_name(module->get_type()), module->get_name().c_str());
  return 0;
}

me::Module* me::MurderEngine::get_module(const string &id) const
{
  for (Module* module : modules)
  {
    /* return module if name matches */
    if (module->get_name() == id)
      return module;
  }

  /* module not found */
  return nullptr;
}

int me::MurderEngine::print_module_exception(const Module &module, const exception &except)
{
  logger.err("received error from module '%s'\n\t%s", module.get_name().c_str(), except.get_message());
  return 0;
}
