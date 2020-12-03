#include "Logger.hpp"

#include <cstdio>
#include <cstdarg>

/* Logger */
me::Logger::Logger(const char* prefix, const Logger* parent)
  : prefix(prefix), parent(parent)
{
}

const char* me::Logger::get_prefix() const
{
  return prefix;
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
  return new Logger(name, this);
}

/* Exception */
me::Exception::Exception(const char* prefix, const bool fatal, const char* format, ...)
  : prefix(prefix), fatal(fatal), message(new char[1024])
{
  va_list args;
  va_start(args, format);
  vsprintf(message, format, args);
  va_end(args);

}

const bool me::Exception::is_fatal() const
{
  return fatal;
}

const char* me::Exception::get_message() const
{
  return message;
}

const char* me::Exception::what() const throw()
{
  return message;
}
