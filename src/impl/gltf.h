#pragma once

// Single place for the tinygltf configuration: these macros change the library's API,
// so the implementation (tinygltf.cpp) and its users (model-gltf.cpp) must agree on them.
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>
