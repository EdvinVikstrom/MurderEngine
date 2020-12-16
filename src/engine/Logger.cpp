#include "Logger.hpp"

#include <stdarg.h>
#include <stdio.h>

/* Logger */
me::Logger::Logger(const char* prefix, const uint8_t tracing, const Logger* parent)
  : prefix(prefix), tracing(tracing), parent(parent)
{
}

#define LOGGER_LOG(p) { \
  va_list args; \
  va_start(args, format); \
  printf(p); \
  printf("[%s] ", prefix); \
  vprintf(format, args); \
  printf("\e[0m\n"); \
  va_end(args); \
}

void me::Logger::fatal(const char* format, ...) const
{
  if ((tracing & FATAL) > 0)
    LOGGER_LOG("\e[31mfatal: ");
}

void me::Logger::err(const char* format, ...) const
{
  if ((tracing & ERR) > 0)
    LOGGER_LOG("\e[31merr: ");
}

void me::Logger::warn(const char* format, ...) const
{
  if ((tracing & WARN) > 0)
    LOGGER_LOG("\e[33mwarn: ");
}

void me::Logger::info(const char* format, ...) const
{
  if ((tracing & INFO) > 0)
    LOGGER_LOG("==> ");
}

void me::Logger::debug(const char* format, ...) const
{
  if ((tracing & DEBUG) > 0)
    LOGGER_LOG("\e[90mdebug: ");
}

#undef LOGGER_LOG

void me::Logger::trace(Trace trace, bool value)
{
  if (value)
    tracing |= trace;
  else
    tracing &= (~trace);
}

void me::Logger::trace_all()
{
  tracing |= FATAL;
  tracing |= ERR;
  tracing |= WARN;
  tracing |= INFO;
  tracing |= DEBUG;
}

me::Logger* me::Logger::child(const char* name) const
{
  return new Logger(name, tracing, this);
}
