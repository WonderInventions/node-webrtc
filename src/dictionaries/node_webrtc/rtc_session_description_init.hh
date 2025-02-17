#pragma once

#include <iosfwd>
#include <string>

#include "src/converters.hh"
#include "src/enums/node_webrtc/rtc_sdp_type.hh"

namespace webrtc {
class SessionDescriptionInterface;
}

// IWYU pragma: no_forward_declare node_webrtc::RTCSessionDescriptionInit
// IWYU pragma: no_include "src/dictionaries/macros/impls.hh"

#define RTC_SESSION_DESCRIPTION_INIT RTCSessionDescriptionInit
#define RTC_SESSION_DESCRIPTION_INIT_LIST                                      \
  DICT_REQUIRED(RTCSdpType, type, "type")                                      \
  DICT_DEFAULT(std::string, sdp, "sdp", "")

#define DICT(X) RTC_SESSION_DESCRIPTION_INIT##X
#include "src/dictionaries/macros/def.hh"
// ordering
#include "src/dictionaries/macros/decls.hh"
#undef DICT

namespace node_webrtc {

static inline RTC_SESSION_DESCRIPTION_INIT
CreateRTCSessionDescriptionInit(const RTCSdpType type, const std::string &sdp) {
  return {type, sdp};
}

DECLARE_CONVERTER(RTCSessionDescriptionInit,
                  webrtc::SessionDescriptionInterface *)
DECLARE_CONVERTER(webrtc::SessionDescriptionInterface *,
                  RTCSessionDescriptionInit)
DECLARE_CONVERTER(const webrtc::SessionDescriptionInterface *,
                  RTCSessionDescriptionInit)

DECLARE_FROM_NAPI(webrtc::SessionDescriptionInterface *)
DECLARE_TO_NAPI(const webrtc::SessionDescriptionInterface *)

} // namespace node_webrtc
