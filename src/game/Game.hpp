#ifndef GAME_HPP
  #define GAME_HPP

#include "../engine/Module.hpp"

class Game : public me::Module {

public:

  Game();

  int initialize(const me::ModuleInfo) override;
  int terminate(const me::ModuleInfo) override;
  int tick(const me::ModuleInfo) override;

};

#endif
