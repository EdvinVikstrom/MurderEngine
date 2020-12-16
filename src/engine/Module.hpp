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

  protected:

    const MurderEngine* engine;
    bool active;

    const Type type;
    const string name;

  public:

    Module(const MurderEngine* engine, const Type type, const string &name)
      : engine(engine), type(type), name(name)
    {
      active = false;
    }

    const Type get_type() const
    { return type; }

    const string& get_name() const
    { return name; }

    const bool is_active() const
    { return active; }


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

    virtual int tick() = 0;

    void enable()
    {
      active = true;
    }

    void disable()
    {
      active = false;
    }

  };

}

#endif
