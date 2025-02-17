#pragma once

#include <cstdint>

// IWYU pragma: no_forward_declare node_webrtc::UnsignedShortRange
// IWYU pragma: no_include "src/dictionaries/macros/impls.hh"

#define UNSIGNED_SHORT_RANGE UnsignedShortRange
#define UNSIGNED_SHORT_RANGE_LIST                                              \
  DICT_OPTIONAL(uint16_t, min, "min")                                          \
  DICT_OPTIONAL(uint16_t, max, "max")

#define DICT(X) UNSIGNED_SHORT_RANGE##X
#include "src/dictionaries/macros/def.hh"
// ordering
#include "src/dictionaries/macros/decls.hh"
#undef DICT
