#include "../memory/MemoryPool.hpp"
#include "../renderer/Renderer.hpp"

#include "Mesh.hpp"

#include "../MurderEngine.hpp"

namespace me {

  class Scene {

  protected:

    MemoryAlloc alloc;
    MemoryPool<Mesh> meshes;

    Renderer* renderer;

  public:

    Scene(const MemoryAlloc &alloc);

    int init(MurderEngine &engine);
    int destroy();

    int tick(MurderEngine &engine);

  };

}
