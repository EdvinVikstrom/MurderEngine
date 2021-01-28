#include "Game.hpp"

Game::Game()
  : Module(me::MODULE_LOGIC_TYPE, "game")
{
}

int Game::initialize(const me::ModuleInfo)
{
  return 0;
}

int Game::terminate(const me::ModuleInfo)
{
  return 0;
}

int Game::tick(const me::ModuleInfo)
{
  return 0;
}
