#pragma once

#include <webrtc/api/ice_transport_interface.h>
#include "src/converters/napi.hh"
#include "src/interfaces/rtc_ice_parameters.hh"   // wrapper we built

namespace node_webrtc {

/**
 * Allow From<Napi::Value> and To<Napi::Value> for webrtc::IceParameters
 *
 *   // From JS object or RTCIceParameters wrapper → native struct
 *   Validation<webrtc::IceParameters> From<Napi::Value>(Napi::Env, Napi::Value);
 *
 *   // Native struct back to plain JS object (rare, but handy for .getStats)
 *   Validation<Napi::Value> To<Napi::Env>(Napi::Env, webrtc::IceParameters);
 */
DECLARE_FROM_NAPI(webrtc::IceParameters)
DECLARE_TO_NAPI(webrtc::IceParameters)

}  // namespace node_webrtc
