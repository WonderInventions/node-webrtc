#pragma once

#include "src/converters/napi.hh"

namespace webrtc {
class Attribute;
}

namespace node_webrtc {

DECLARE_TO_NAPI(const webrtc::Attribute *)

} // namespace node_webrtc
