// tinygltf implementation. Deliberately not a module unit: its internal-linkage helpers
// are not found by clang when the templates are instantiated inside a module purview.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "gltf.h"
