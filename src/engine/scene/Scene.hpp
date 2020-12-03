#include "../memory/MemoryPool.hpp"
#include "../renderer/Renderer.hpp"

#include "Mesh.hpp"

#include "../MurderEngine.hpp"

namespace me {

  class Scene {

  protected:

    MemPool<Mesh>* meshes;

    Renderer* renderer;

  public:

    Scene();

    int init(MurderEngine &engine);
    int destroy();

    int tick(MurderEngine &engine);

  };

}
