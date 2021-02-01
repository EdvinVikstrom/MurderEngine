#include "obj_format.hpp"

#include <lme/string_buffer.hpp>

me::OBJFormat::OBJFormat()
  : Format(FORMAT_TYPE_MESH, "wavefront:obj")
{
}

int me::OBJFormat::read(const me::File &file, size_t len, const char* data, Result &result)
{
  string_buffer buffer(len, data);

  MeshItem* mesh_item = nullptr;
  size_t position_index = 0, normal_index = 0, texture_index = 0, color_index = 0, face_index = 0;
  vector<Vertex> vertices;
  vector<Index> indices;
  while (buffer.has_next())
  {
    buffer.skip_whitespace();
    char c = buffer.next_char();

    if (c == '#') /* skip comments */
    {
      buffer.skip_line();
    }else if (c == 'o')
    {
      if (mesh_item != nullptr)
	result.meshes.push_back(mesh_item);
      mesh_item = new MeshItem;
      mesh_item->identifier = buffer.next_literial();
      position_index = 0;
      texture_index = 0;
      color_index = 0;
      buffer.skip_line();
    }else if (c == 'v')
    {
      if (buffer.peek_char() == 'n')
      {
	buffer.next_char(); /* skipping the 'n' */

	size_t pos_len;
	const char* pos_str;

	pos_str = buffer.next_float(pos_len);
	float x = strfloat(pos_len, pos_str);

	pos_str = buffer.next_float(pos_len);
	float y = strfloat(pos_len, pos_str);

	pos_str = buffer.next_float(pos_len);
	float z = strfloat(pos_len, pos_str);

	vertices[normal_index++].normal = {x, y, z};
      }else if (buffer.peek_char() == 't')
      {
	buffer.next_char(); /* skipping the 't' */

	size_t pos_len;
	const char* pos_str;

	pos_str = buffer.next_float(pos_len);
	float x = strfloat(pos_len, pos_str);

	pos_str = buffer.next_float(pos_len);
	float y = strfloat(pos_len, pos_str);

	vertices[texture_index++].tex_coord = {x, y};
      }else
      {
	size_t pos_len;
	const char* pos_str;

	pos_str = buffer.next_float(pos_len);
	float x = strfloat(pos_len, pos_str);

	pos_str = buffer.next_float(pos_len);
	float y = strfloat(pos_len, pos_str);

	pos_str = buffer.next_float(pos_len);
	float z = strfloat(pos_len, pos_str);

	vertices[position_index++].position = {x, y, z};
      }
      buffer.skip_line();
    }else if (c == 'f')
    {
      uint32_t indices[12];

      for (uint8_t i = 0; i < 4; i++)
      {
	size_t index_len;
	const char* index_str;

	index_str = buffer.next_integer(index_len);
	uint32_t x = struint32(index_len, index_str);

	indices[face_index++] = x - 1;
      }
    }

  }

  return 0;
}

bool me::OBJFormat::recognize(const me::File &file)
{
  if (!file.has_suffix())
    return false;
  const string suffix = file.get_suffix();
  return suffix == "obj";
}
