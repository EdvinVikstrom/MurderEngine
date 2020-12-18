#ifndef ME_MODULE_HPP
  #define ME_MODULE_HPP

#include "MurderEngine.hpp"

#include <lme/string.hpp>

namespace me {

  class Module {

    friend MurderEngine;

  public:

    enum Type {
      SURFACE, RENDERER, AUDIO, IO, OTHER
    };

    struct Context {
      size_t current_tick;
      size_t current_time;
    };

  protected:

    const MurderEngine* engine;
    const Type type;
    const string name;

    mutable size_t sleep;

  public:

    Module(const MurderEngine* engine, const Type type, const string &name, size_t sleep = 0)
      : engine(engine), type(type), name(name), sleep(sleep)
    {
    }

    const Type get_type() const
    {
      return type;
    }

    const string& get_name() const
    {
      return name;
    }


    static inline const char* type_name(const Type type)
    {
      switch (type)
      {
	case RENDERER: return "RENDERER";
	case AUDIO: return "AUDIO";
	case SURFACE: return "SURFACE";
	case IO: return "IO";
	case OTHER: return "OTHER";
	default: return "";
      }
    }

  protected:

    virtual int initialize() = 0;
    virtual int terminate() = 0;
    virtual int tick(const Context context) = 0;

  };

}

#endif
