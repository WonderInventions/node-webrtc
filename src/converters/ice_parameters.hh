#pragma once

#include <webrtc/p2p/base/transport_description.h>
#include "src/converters/napi.hh"
#include "src/interfaces/rtc_ice_parameters.hh"   // wrapper we built

namespace node_webrtc {

DECLARE_FROM_NAPI(cricket::IceParameters)
DECLARE_TO_NAPI(cricket::IceParameters)

}  // namespace node_webrtc
