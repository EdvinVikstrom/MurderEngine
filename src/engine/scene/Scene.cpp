#include "Scene.hpp"

me::Scene::Scene()
{
  meshes = new MemPool<Mesh>(1024);
}

int me::Scene::init(MurderEngine &engine)
{
  return 0;
}

int me::Scene::destroy()
{
  return 0;
}

int me::Scene::tick(MurderEngine &engine)
{
  return 0;
}
