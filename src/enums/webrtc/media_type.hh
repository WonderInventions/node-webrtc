#pragma once

#include <webrtc/api/media_types.h>

// IWYU pragma: no_include "src/enums/macros/impls.hh"

// FIXME(mroberts): I'm not sure that "data" should be valid.
#define WEBRTC_MEDIA_TYPE webrtc::MediaType
#define WEBRTC_MEDIA_TYPE_NAME "kind"
#define WEBRTC_MEDIA_TYPE_LIST                                                 \
  ENUM_SUPPORTED(WEBRTC_MEDIA_TYPE::AUDIO, "audio")                           \
  ENUM_SUPPORTED(WEBRTC_MEDIA_TYPE::VIDEO, "video")                           \
  ENUM_SUPPORTED(WEBRTC_MEDIA_TYPE::DATA, "data")                             \
  ENUM_UNSUPPORTED(WEBRTC_MEDIA_TYPE::UNSUPPORTED, "unsupported",             \
                   "Unsupported media type is not supported")

#define ENUM(X) WEBRTC_MEDIA_TYPE##X
#include "src/enums/macros/decls.hh"
#undef ENUM
