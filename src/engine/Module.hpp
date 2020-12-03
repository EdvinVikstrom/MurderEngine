#ifndef ME_MODULE_HPP
  #define ME_MODULE_HPP

#include "MurderEngine.hpp"

#include <string>
#include <queue>

namespace me {

  class Module {

    friend MurderEngine;

  public:

    enum Type {
      SURFACE, RENDERER, IO, OTHER
    };

  protected:

    const MurderEngine* engine;
    bool active;

    const Type type;
    const std::string name;

    std::queue<void*> buffer;

    /* pull data from the beginning of buffer */
    void* pull_data()
    {
      void* value = buffer.front();
      buffer.pop();
      return value;
    }

  public:

    Module(const MurderEngine* engine, const Type type, const std::string &name)
      : engine(engine), type(type), name(name)
    {
      active = false;
    }

    const Type get_type() const
    { return type; }

    const std::string& get_name() const
    { return name; }

    const bool is_active() const
    { return active; }

    /* push data to the end of buffer */
    void push_data(void* data)
    { buffer.push(data); }
    

    virtual int signal() = 0;

    virtual int initialize() = 0;
    virtual int terminate() = 0;

    virtual int tick() = 0;

    static inline const char* type_name(const Type type)
    {
      switch (type)
      {
	case RENDERER: return "RENDERER";
	case SURFACE: return "SURFACE";
	case IO: return "IO";
	case OTHER: return "OTHER";
	default: return "";
      }
    }

  protected:

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
