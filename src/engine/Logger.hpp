#ifndef ME_LOGGER_HPP
  #define ME_LOGGER_HPP

#include <lme/array_proxy.hpp>

namespace me {

  enum LogVisibility {
    LOG_VISIBILITY_PUBLIC,
    LOG_VISIBILITY_PRIVATE
  };

  enum LogFlag {
    LOG_FATAL_FLAG = 1,
    LOG_ERR_FLAG = 1 << 1,
    LOG_WARN_FLAG = 1 << 2,
    LOG_INFO_FLAG = 1 << 3,
    LOG_DEBUG_FLAG = 1 << 4,

    LOG_EXPORT_FLAG = 1 << 5,
    LOG_WRITE_TTY_FLAG = 1 << 6
  };


  class Logger {

  private:

    const char* prefix;
    uint8_t flags;
    const Logger* parent;

  public:

    Logger(const char* prefix, const uint8_t flags = (LOG_FATAL_FLAG | LOG_ERR_FLAG | LOG_WARN_FLAG | LOG_INFO_FLAG | LOG_EXPORT_FLAG), const Logger* parent = nullptr);

    void fatal(const char* format, ...) const;
    void err(const char* format, ...) const;
    void warn(const char* format, ...) const;
    void info(const char* format, ...) const;
    void debug(const char* format, ...) const;

    uint8_t q_choose(const array_proxy<const char*> &options, uint8_t default_opt, const char* format, ...);

    void set_option(LogFlag flag, bool value);
    
    static int init(FILE* ext_log);

  };

}

#endif
