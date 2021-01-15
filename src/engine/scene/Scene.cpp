#include "Scene.hpp"

me::Scene::Scene(const MemoryAlloc &allocator)
  : allocator(allocator.child(1024 * 16))
{
  meshes = MemoryPool<Mesh>(this->allocator);
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
