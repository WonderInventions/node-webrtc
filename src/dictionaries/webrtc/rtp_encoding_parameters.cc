#include "src/dictionaries/webrtc/rtp_encoding_parameters.hh"

#include <cstdint>
#include <iosfwd>
#include <string>

#include <webrtc/api/rtp_parameters.h>

#include "src/converters.hh"
#include "src/converters/absl.hh"
#include "src/converters/object.hh"
#include "src/dictionaries/macros/napi.hh"
#include "src/enums/node_webrtc/rtc_dtx_status.hh"
#include "src/enums/node_webrtc/rtc_priority_type.hh"
#include "src/functional/maybe.hh"
#include "src/functional/validation.hh"

namespace node_webrtc {

#define RTP_ENCODING_PARAMETERS_FN CreateRtpEncodingParameters
#define RTP_ENCODING_PARAMETERS_LIST                                           \
  DICT_OPTIONAL(std::string, rid, "rid")                                       \
  DICT_OPTIONAL(uint32_t, ssrc, "ssrc")                                        \
  DICT_OPTIONAL(uint8_t, codecPayloadType, "codecPayloadType")                 \
  DICT_OPTIONAL(node_webrtc::RTCDtxStatus, dtx, "dtx")                         \
  DICT_DEFAULT(bool, active, "active", true)                                   \
  DICT_DEFAULT(RTCPriorityType, priority, "priority", RTCPriorityType::kLow)   \
  DICT_OPTIONAL(uint64_t, ptime, "ptime")                                      \
  DICT_OPTIONAL(uint64_t, maxBitrate, "maxBitrate")                            \
  DICT_OPTIONAL(double, maxFramerate, "maxFramerate")                          \
  DICT_OPTIONAL(double, scaleResolutionDownBy, "scaleResolutionDownBy")

static Validation<webrtc::RtpEncodingParameters> RTP_ENCODING_PARAMETERS_FN(
    const node_webrtc::Maybe<std::string> &rid,
    const node_webrtc::Maybe<uint32_t> ssrc,
    const node_webrtc::Maybe<uint8_t> codecPayloadType,
    const node_webrtc::Maybe<node_webrtc::RTCDtxStatus> dtx, const bool active,
    const RTCPriorityType priority, const node_webrtc::Maybe<uint64_t> ptime,
    const node_webrtc::Maybe<uint64_t> maxBitrate,
    const node_webrtc::Maybe<double> maxFramerate,
    const node_webrtc::Maybe<double> scaleResolutionDownBy) {
  // NOTE(mroberts): No longer supported in WebRTC 1.0.
  (void)codecPayloadType;
  (void)dtx;
  (void)priority;
  (void)ptime;

  webrtc::RtpEncodingParameters parameters;
  if (rid.IsJust()) {
    parameters.rid = rid.UnsafeFromJust();
  }
  if (ssrc.IsJust()) {
    parameters.ssrc = ssrc.UnsafeFromJust();
  }
  parameters.active = active;
  if (maxBitrate.IsJust()) {
    parameters.max_bitrate_bps = maxBitrate.UnsafeFromJust();
  }
  if (maxFramerate.IsJust()) {
    parameters.max_framerate = maxFramerate.UnsafeFromJust();
  }
  if (scaleResolutionDownBy.IsJust()) {
    parameters.scale_resolution_down_by =
        scaleResolutionDownBy.UnsafeFromJust();
  }
  return Pure(parameters);
}

TO_NAPI_IMPL(webrtc::RtpEncodingParameters, pair) {
  auto env = pair.first;
  Napi::EscapableHandleScope scope(env);
  auto parameters = pair.second;
  NODE_WEBRTC_CREATE_OBJECT_OR_RETURN(env, object)
  if (!parameters.rid.empty()) {
    NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "rid", parameters.rid)
  }
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "active",
                                        parameters.active)
  if (parameters.max_bitrate_bps) {
    NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "maxBitrate",
                                          parameters.max_bitrate_bps)
  }
  if (parameters.scale_resolution_down_by) {
    NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "scaleResolutionDownBy",
                                          parameters.scale_resolution_down_by)
  }
  return Pure(scope.Escape(object));
}

} // namespace node_webrtc

#define DICT(X) RTP_ENCODING_PARAMETERS##X
#include "src/dictionaries/macros/impls.hh"
