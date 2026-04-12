#include "src/dictionaries/node_webrtc/rtc_session_description_init.hh"

#include <utility>

#include <node-addon-api/napi.h>
#include <webrtc/api/jsep.h>

#include "src/dictionaries/macros/napi.hh"
#include "src/functional/curry.hh"
#include "src/functional/operators.hh"
#include "src/functional/validation.hh"

namespace node_webrtc {

#define RTC_SESSION_DESCRIPTION_INIT_FN CreateValidRTCSessionDescriptionInit

static webrtc::SdpType ToWebrtcSdpType(const RTCSdpType type) {
  switch (type) {
  case RTCSdpType::kOffer:
    return webrtc::SdpType::kOffer;
  case RTCSdpType::kPrAnswer:
    return webrtc::SdpType::kPrAnswer;
  case RTCSdpType::kAnswer:
    return webrtc::SdpType::kAnswer;
  case RTCSdpType::kRollback:
    return webrtc::SdpType::kRollback;
  }
  return webrtc::SdpType::kOffer;
}

static Validation<RTC_SESSION_DESCRIPTION_INIT>
RTC_SESSION_DESCRIPTION_INIT_FN(const RTCSdpType type, const std::string &sdp) {
  return Pure(CreateRTCSessionDescriptionInit(type, sdp));
}

TO_NAPI_IMPL(RTCSessionDescriptionInit, pair) {
  auto env = pair.first;
  Napi::EscapableHandleScope scope(env);
  NODE_WEBRTC_CREATE_OBJECT_OR_RETURN(env, object)
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "type", pair.second.type)
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "sdp", pair.second.sdp)
  return Pure(scope.Escape(object));
}

CONVERTER_IMPL(RTCSessionDescriptionInit, webrtc::SessionDescriptionInterface *,
               init) {
  webrtc::SdpParseError error;
  auto description =
      webrtc::CreateSessionDescription(ToWebrtcSdpType(init.type), init.sdp,
                                       &error);
  if (!description) {
    return Validation<webrtc::SessionDescriptionInterface *>::Invalid(
        error.description);
  }
  return Pure(description.release());
}

CONVERTER_IMPL(webrtc::SessionDescriptionInterface *, RTCSessionDescriptionInit,
               description) {
  return Converter<const webrtc::SessionDescriptionInterface *,
                   RTCSessionDescriptionInit>::Convert(description);
}

CONVERTER_IMPL(const webrtc::SessionDescriptionInterface *,
               RTCSessionDescriptionInit, description) {
  if (!description) {
    return Validation<RTCSessionDescriptionInit>::Invalid(
        "RTCSessionDescription is null");
  }
  std::string sdp;
  if (!description->ToString(&sdp)) {
    return Validation<RTCSessionDescriptionInit>::Invalid(
        "Failed to print the SDP. This is pretty weird. File a bug on "
        "https://github.com/node-webrtc/node-webrtc");
  }
  return curry(CreateRTCSessionDescriptionInit) %
         From<RTCSdpType>(description->type()) * Pure(sdp);
}

FROM_NAPI_IMPL(webrtc::SessionDescriptionInterface *, pair) {
  return From<RTCSessionDescriptionInit>(pair)
      .FlatMap<webrtc::SessionDescriptionInterface *>(
          Converter<RTCSessionDescriptionInit,
                    webrtc::SessionDescriptionInterface *>::Convert);
}

TO_NAPI_IMPL(const webrtc::SessionDescriptionInterface *, pair) {
  return From<RTCSessionDescriptionInit>(pair.second)
      .FlatMap<Napi::Value>([env = pair.first](auto value) {
        return From<Napi::Value>(std::make_pair(env, value));
      });
}

} // namespace node_webrtc

#define DICT(X) RTC_SESSION_DESCRIPTION_INIT##X
#include "src/dictionaries/macros/impls.hh"
#undef DICT
