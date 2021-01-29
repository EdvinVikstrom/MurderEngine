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

const char* me::descriptor_type_name(DescriptorType type)
{
  switch (type)
  {
    case DESCRIPTOR_TYPE_UNIFORM: return "DESCRIPTOR_TYPE_UNIFORM";
    case DESCRIPTOR_TYPE_NONE: return "DESCRIPTOR_TYPE_NONE";
    default: return "?";
  }
}
