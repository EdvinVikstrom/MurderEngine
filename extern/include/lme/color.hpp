#ifndef LIBME_COLOR_HPP
  #define LIBME_COLOR_HPP

#include "math/vector.hpp"

namespace me {

  struct Color3f {

    union {
      float red, green, blue;
    };

    Color3f(float red = 0.0F, float green = 0.0F, float blue = 0.0F)
    {
      this->red = red;
      this->green = green;
      this->blue = blue;
    }

    Color3f(uint32_t i)
    {
      red = (float) (i & 0xFF) / 255.0F;
      green = (float) ((i << 8) & 0xFF) / 255.0F;
      blue = (float) ((i << 16) & 0xFF) / 255.0F;
    }


    [[nodiscard]] Color3f invert() const
    {
      return { 1.0F - red, 1.0F - green, 1.0F - blue };
    }

  };

  struct Color4f {

    union {
      float red, green, blue, alpha;
    };

    Color4f(float red = 0.0F, float green = 0.0F, float blue = 0.0F, float alpha = 1.0F)
    {
      this->red = red;
      this->green = green;
      this->blue = blue;
      this->alpha = alpha;
    }

    Color4f(uint32_t i)
    {
      red = (float) (i & 0xFF) / 255.0F;
      green = (float) ((i << 8) & 0xFF) / 255.0F;
      blue = (float) ((i << 16) & 0xFF) / 255.0F;
      alpha = (float) ((i << 24) & 0xFF) / 255.0F;
    }

    [[nodiscard]] Color4f invert() const
    {
      return { 1.0F - red, 1.0F - green, 1.0F - blue, 1.0F - alpha };
    }

  };


  typedef Color3f col3f;
  typedef Color4f col4f;

}

#endif
