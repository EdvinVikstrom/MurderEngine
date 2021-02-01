#ifndef ME_IMAGE_HPP
  #define ME_IMAGE_HPP

namespace me {

  enum ImageFormat {
    IMAGE_FORMAT_RGB8,
    IMAGE_FORMAT_RGBA8
  };


  class Image {

  public:

    ImageFormat format;
    uint32_t width, height;
    uint8_t* data;

  };

}

#endif
