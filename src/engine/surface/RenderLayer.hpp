#ifndef ME_RENDER_LAYER_HPP
  #define ME_RENDER_LAYER_HPP

namespace me {

  class RenderLayer {

  public:

    virtual int render(bool resized) = 0;

  };

}

#endif
