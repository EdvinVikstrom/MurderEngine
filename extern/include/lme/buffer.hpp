#ifndef LIBME_BYTEBUFF_HPP
  #define LIBME_BYTEBUFF_HPP

#include "string.hpp"

namespace me {

  enum ByteOrder {
    BYTE_ORDER_LITTLE_ENDIAN,
    BYTE_ORDER_BIG_ENDIAN
  };


  class Buffer {

  public:

    enum Flag {
      FLAG_READ = 1,
      FLAG_WRITE = 1 << 1
    };

  protected:

    const ByteOrder order;
    uint8_t state;

  public:

    explicit Buffer(const ByteOrder order = BYTE_ORDER_LITTLE_ENDIAN, uint8_t state = FLAG_READ | FLAG_WRITE);

    virtual inline Buffer& push(const char data) = 0;
    virtual inline Buffer& push(const char* data, const size_t size) = 0;

    Buffer& push_int8(int8_t i);
    Buffer& push_int16(int16_t i);
    Buffer& push_int32(int32_t i);
    Buffer& push_int64(int64_t i);

    Buffer& push_uint8(uint8_t i);
    Buffer& push_uint16(uint16_t i);
    Buffer& push_uint32(uint32_t i);
    Buffer& push_uint64(uint64_t i);

    Buffer& push_string(const char* str, size_t len = -1);


    virtual inline const char pull() const = 0;
    virtual inline const char* pull(const size_t size) const = 0;
    virtual inline const char* pull_all() const = 0;
    virtual inline const char peek() const = 0;
    virtual inline const char* peek(const size_t size) const = 0;
    virtual inline const size_t seek(char c) const = 0;
    virtual inline const size_t seek_end() const = 0;

    int8_t pull_int8() const;
    int16_t pull_int16() const;
    int32_t pull_int32() const;
    int64_t pull_int64() const;

    uint8_t pull_uint8() const;
    uint16_t pull_uint16() const;
    uint32_t pull_uint32() const;
    uint64_t pull_uint64() const;

    const char* pull_string() const;



    virtual inline bool has() const = 0;
    virtual inline size_t position() const = 0;

  };

  class ByteBuffer : public Buffer {

  protected:

    const size_t size;
    char* data;

    mutable size_t pos = 0;

  public:

    ByteBuffer(const size_t size, char* data, const ByteOrder order = BYTE_ORDER_LITTLE_ENDIAN, uint8_t state = FLAG_READ | FLAG_WRITE);

    inline Buffer& push(const char data) override;
    inline Buffer& push(const char* data, const size_t size) override;

    inline const char pull() const override;
    inline const char* pull(const size_t size) const override;
    inline const char* pull_all() const override;
    inline const char peek() const override;
    inline const char* peek(size_t size) const override;
    inline const size_t seek(char c) const override;
    inline const size_t seek_end() const override;

    inline bool has() const override;
    inline size_t position() const override;

  };

}

#endif
