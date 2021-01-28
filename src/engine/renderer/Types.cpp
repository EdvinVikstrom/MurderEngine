#include "Types.hpp"

const char* me::structure_type_name(StructureType type)
{
  switch (type)
  {
    case STRUCTURE_TYPE_SURFACE_CREATE_INFO: return "STRUCTURE_TYPE_SURFACE_CREATE_INFO";
    case STRUCTURE_TYPE_DEVICE_CREATE_INFO: return "STRUCTURE_TYPE_DEVICE_CREATE_INFO";
    case STRUCTURE_TYPE_NONE: return "STRUCTURE_TYPE_NONE";
    default: return "?";
  }
}
