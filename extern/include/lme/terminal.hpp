#ifndef LIBME_TERMINAL_HPP
  #define LIBME_TERMINAL_HPP

namespace me::terminal {

  static const char* CURSOR_UP = "\e[%uA";
  static const char* CURSOR_DOWN = "\e[%uB";
  static const char* CURSOR_FORWARD = "\e[%uC";
  static const char* CURSOR_BACK = "\e[%uD";
  static const char* CURSOR_NEXT_LINE = "\e[%uE";
  static const char* CURSOR_PREV_LINE = "\e[%uF";
  static const char* CURSOR_POS = "\e[%u;%uH";

  static const char* ERASE_DISPLAY = "\e[%uJ";
  static const char* ERASE_LINE = "\e[%uK";

  static const char* SHOW_CURSOR = "\e[25h";
  static const char* HIDE_CURSOR = "\e[25l";

  static const char* ENABLE_ALT_BUFFER = "\e[1049h";
  static const char* DISABLE_ALT_BUFFER = "\e[1049l";

}

#endif
