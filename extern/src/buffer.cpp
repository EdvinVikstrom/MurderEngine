#include "buffer.hpp"

#include "algorithm.hpp"

using namespace me;

/* class Buffer */
Buffer::Buffer(const ByteOrder order, uint8_t state)
  : order(order)
{
  this->state = state;
}

Buffer& Buffer::push_int8(int8_t i)
{
  push(i);
  return *this;
}

Buffer& Buffer::push_int16(int16_t i)
{
  char data[2];
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
  {
    data[0] = i & 0xFF;
    data[1] = (i >> 8) & 0xFF;
  }else
  {
    data[0] = (i >> 8) & 0xFF;
    data[1] = i & 0xFF;
  }
  push(data, 2);
  return *this;
}

Buffer& Buffer::push_int32(int32_t i)
{
  char data[4];
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
  {
    data[0] = i & 0xFF;
    data[1] = (i >> 8) & 0xFF;
    data[2] = (i >> 16) & 0xFF;
    data[3] = (i >> 24) & 0xFF;
  }else
  {
    data[0] = (i >> 24) & 0xFF;
    data[1] = (i >> 16) & 0xFF;
    data[2] = (i >> 8) & 0xFF;
    data[3] = i & 0xFF;
  }
  push(data, 4);
  return *this;
}

Buffer& Buffer::push_int64(int64_t i)
{
  char data[8];
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
  {
    data[0] = i & 0xFF;
    data[1] = (i >> 8) & 0xFF;
    data[2] = (i >> 16) & 0xFF;
    data[3] = (i >> 24) & 0xFF;
    data[4] = (i >> 32) & 0xFF;
    data[5] = (i >> 40) & 0xFF;
    data[6] = (i >> 48) & 0xFF;
    data[7] = (i >> 56) & 0xFF;
  }else
  {
    data[0] = (i >> 56) & 0xFF;
    data[1] = (i >> 48) & 0xFF;
    data[2] = (i >> 40) & 0xFF; 
    data[3] = (i >> 32) & 0xFF;
    data[4] = (i >> 24) & 0xFF;
    data[5] = (i >> 16) & 0xFF;
    data[6] = (i >> 8) & 0xFF;
    data[7] = i & 0xFF;
  }
  push(data, 8);
  return *this;
}

Buffer& Buffer::push_uint8(uint8_t i)
{
  push((char) (i));
  return *this;
}

Buffer& Buffer::push_uint16(uint16_t i)
{
  return push_int16((int16_t) (i));
}

Buffer& Buffer::push_uint32(uint32_t i)
{
  return push_int32((int32_t) (i));
}

Buffer& Buffer::push_uint64(uint64_t i)
{
  return push_int64((int64_t) (i));
}

Buffer& Buffer::push_string(const char* str, size_t len)
{
  if (len == -1)
    len = strlen(str);

  push(str, len);
  push('\0');
  return *this;
}


int8_t Buffer::pull_int8() const
{
  return pull();
}

int16_t Buffer::pull_int16() const
{
  int16_t i = 0x0000;
  const char* data = pull(2);
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
    i |= (int16_t) data[0] | (int16_t) data[1] << 8;
  else
    i |= (int16_t) data[0] << 8 | (uint16_t) data[1];
  return i;
}

int32_t Buffer::pull_int32() const
{
  int32_t i = 0x00000000;
  const char* data = pull(4);
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
    i = (int32_t) data[0] | (int32_t) data[1] << 8 | (int32_t) data[2] << 16 | (int32_t) data[3] << 24;
  else
    i = (int32_t) data[0] << 24 | (int32_t) data[1] << 16 | (int32_t) data[2] << 8 | (int32_t) data[3];
  return i;
}

int64_t Buffer::pull_int64() const
{
  int64_t i = 0x0000000000000000;
  const char* data = pull(8);
  if (order == BYTE_ORDER_LITTLE_ENDIAN)
    i = (int64_t) data[0] | (int64_t) data[1] << 8 | (int64_t) data[2] << 16 | (int64_t) data[3] << 24 |
      (int64_t) data[4] << 32 | (int64_t) data[5] << 40 | (int64_t) data[6] << 48 | (int64_t) data[7] << 56;
  else
    i = (int64_t) data[0] << 56 | (int64_t) data[1] << 48 | (int64_t) data[2] << 40 | (int64_t) data[3] << 32 |
      (int64_t) data[4] << 24 | (int64_t) data[5] << 16 | (int64_t) data[6] << 8 || (int64_t) data[7];
  return i;
}

uint8_t Buffer::pull_uint8() const
{
  return (uint8_t) (pull());
}

uint16_t Buffer::pull_uint16() const
{
  return (uint16_t) (pull_int16());
}

uint32_t Buffer::pull_uint32() const
{
  return (uint32_t) (pull_int32());
}

uint64_t Buffer::pull_uint64() const
{
  return (uint64_t) (pull_int64());
}

const char* Buffer::pull_string() const
{
  return pull(1);
}
/* end class Buffer */



/* class ByteBuffer */
ByteBuffer::ByteBuffer(const size_t size, char* data, const ByteOrder order, uint8_t state)
  : Buffer(order, state), size(size)
{
  this->data = data;
}

inline Buffer& ByteBuffer::push(const char data)
{
  if (!(state & FLAG_WRITE))
    return *this;

  this->data[pos++] = data;
  return *this;
}

inline Buffer& ByteBuffer::push(const char* data, const size_t size)
{
  if (!(state & FLAG_WRITE))
    return *this;

  for (size_t i = 0; i < size; i++)
    this->data[pos++] = data[i];
  return *this;
}

inline const char ByteBuffer::pull() const
{
  if (!(state & FLAG_READ))
    return '\0';
  return data[pos++];
}

inline const char* ByteBuffer::pull(const size_t size) const
{
  if (!(state & FLAG_READ))
    return nullptr;
  return &data[pos += size];
}

inline const char* ByteBuffer::pull_all() const
{
  pos += size;
  return data;
}

inline const char ByteBuffer::peek() const
{
  if (!(state & FLAG_READ))
    return '\0';

  return data[pos];
}

inline const char* ByteBuffer::peek(size_t size) const
{
  if (!(state & FLAG_READ))
    return nullptr;
  return &data[pos];
}

inline const size_t ByteBuffer::seek(char c) const
{
  if (!(state & FLAG_READ))
    return -1;

  for (size_t i = pos; i < size; i++)
  {
    if (data[i] == c)
      return i;
  }
  return -1;
}

inline const size_t ByteBuffer::seek_end() const
{
  return size;
}

inline bool ByteBuffer::has() const
{
  return pos < size;
}

inline size_t ByteBuffer::position() const
{
  return pos;
}
/* end class ByteBuffer */
