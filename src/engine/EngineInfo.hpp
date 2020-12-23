#ifndef ME_ENGINE_INFO_HPP
  #define ME_ENGINE_INFO_HPP

#ifndef ME_ENGINE_NAME
  #define ME_ENGINE_NAME 		"MurderEngine"
#endif
#ifndef ME_ENGINE_VERSION
  #define ME_ENGINE_VERSION_NAME	"202012"
  #define ME_ENGINE_VERSION_MAJOR	0
  #define ME_ENGINE_VERSION_MINOR	1
  #define ME_ENGINE_VERSION_PATCH	0
#endif

namespace me {

  struct ApplicationInfo {
    const char* name;
    const uint32_t version;
  };

  struct EngineInfo {
    ApplicationInfo application_info;
    uint32_t module_count;
    class Module** modules;
  };

}

#endif
