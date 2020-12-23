#ifndef ME_LOGGER_HPP
  #define ME_LOGGER_HPP

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

    Logger(const char* prefix, const uint8_t tracing = (FATAL | ERR | WARN | INFO), const Logger* parent = nullptr);

    void fatal(const char* format, ...) const;
    void err(const char* format, ...) const;
    void warn(const char* format, ...) const;
    void info(const char* format, ...) const;
    void debug(const char* format, ...) const;

    void trace(Trace trace, bool value);
    void trace_all();

  };

}

#endif
