#include "MurderEngine.hpp"

#include "Module.hpp"
#include "Common.hpp"

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
      }catch (const Exception &e)
      {
	if (e.is_fatal())
	{
	  logger.fatal("%s", e.get_message());
	  terminate();
	}else
	  logger.err("%s", e.get_message());
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
  }catch (const Exception &e)
  {
    if (e.is_fatal())
    {
      logger.fatal("%s", e.get_message());
      terminate();
    }else
      logger.err("%s", e.get_message());
    return 1;
  }

  logger.debug("loaded %s module '%s'", Module::type_name(module->get_type()), module->get_name().c_str());
  return 0;
}

me::Module* me::MurderEngine::get_module(const std::string &id) const
{
  for (Module* module : modules)
  {
    /* return module if name matches */
    if (module->get_name().compare(id) == 0)
      return module;
  }

  /* module not found */
  return nullptr;
}
