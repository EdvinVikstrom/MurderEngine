#ifndef ME_LOGGER_HPP
  #define ME_LOGGER_HPP

#include <string>

namespace me {

  class Logger {

  public:

    enum Trace {
      FATAL = 1,
      ERR = 1 << 1,
      WARN = 1 << 2,
      INFO = 1 << 3,
      DEBUG = 1 << 4
    };

  private:

    const char* prefix;
    uint8_t tracing;
    const Logger* parent;

  public:

    Logger(const char* prefix, const Logger* parent = nullptr);

    const char* get_prefix() const;

    void fatal(const char* format, ...) const;
    void err(const char* format, ...) const;
    void warn(const char* format, ...) const;
    void info(const char* format, ...) const;
    void debug(const char* format, ...) const;

    void trace(Trace trace, bool value);
    void trace_all();

    Logger* child(const char* name) const;

  };

  class Exception : public std::exception {

  protected:

    const char* prefix;
    const bool fatal;
    char* const message;

  public:

    Exception(const char* prefix, const bool fatal, const char* format, ...);

    const bool is_fatal() const;
    const char* get_message() const;

    const char* what() const throw();

  };

}

#endif
