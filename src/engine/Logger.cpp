#include "Logger.hpp"

#include <lme/string.hpp>

#include <stdarg.h>
#include <stdio.h>

static FILE* log_file = nullptr;


/* Logger */
me::Logger::Logger(const char* prefix, const uint8_t flags, const Logger* parent)
  : prefix(prefix), flags(flags), parent(parent)
{
}

#define LOGGER_PRINT_FORMAT(f) { \
  va_list args; \
  va_start(args, f); \
  vprintf(f, args); \
  va_end(args); \
}

#define LOGGER_LOG_FILE(p, f) { \
  va_list args; \
  va_start(args, format); \
  fprintf(f, p); \
  fprintf(f, "[%s] ", prefix); \
  vfprintf(f, format, args); \
  fprintf(f, "\e[0m\n"); \
  va_end(args); \
}

#define LOGGER_LOG(p) { \
  LOGGER_LOG_FILE(p, stdout); \
  if (flags & LOG_EXPORT_FLAG && log_file != nullptr) \
    LOGGER_LOG_FILE(p, log_file) \
}

void me::Logger::fatal(const char* format, ...) const
{
  if (flags & LOG_FATAL_FLAG)
    LOGGER_LOG("\e[31mfatal: ");
}

void me::Logger::err(const char* format, ...) const
{
  if (flags & LOG_ERR_FLAG)
    LOGGER_LOG("\e[31merr: ");
}

void me::Logger::warn(const char* format, ...) const
{
  if (flags & LOG_WARN_FLAG)
    LOGGER_LOG("\e[33mwarn: ");
}

void me::Logger::info(const char* format, ...) const
{
  if (flags & LOG_INFO_FLAG)
    LOGGER_LOG("==> ");
}

void me::Logger::debug(const char* format, ...) const
{
  if (flags & LOG_DEBUG_FLAG)
    LOGGER_LOG("\e[90mdebug: ");
}

uint8_t me::Logger::q_choose(const array_proxy<const char*> &options, uint8_t default_opt, const char* format, ...)
{
  LOGGER_PRINT_FORMAT(format);
  putchar('\n');

  for (size_t i = 0; i < options.size(); i++)
  {
    printf("  %s (%lu)\n", options[i], i);
  }

  do {
    printf("pick a option (default: %u): ", default_opt);

    char input[4];
    fgets(input, 4, stdin);
    size_t len = strlen(input);

    if (len == 0)
      return default_opt;

    uint8_t opt = struint8(len, input);
    if (opt < options.size())
      return opt;

    printf("index out of range \e[31m%u\e[0m > %u\n", opt, (uint8_t) options.size() - 1);
  }while (true);

}

#undef LOGGER_LOG

void me::Logger::set_option(LogFlag flag, bool value)
{
  if (value)
    flags |= flag;
  else
    flags &= (~flag);
}

int me::Logger::init(FILE* ext_log)
{
  log_file = ext_log;
  return 0;
}
