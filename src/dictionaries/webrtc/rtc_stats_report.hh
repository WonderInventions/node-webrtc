#pragma once

#include "src/converters/napi.hh"

namespace webrtc {
class RTCStatsReport;
template <typename T> class scoped_refptr;
}

namespace node_webrtc {

DECLARE_TO_NAPI(webrtc::scoped_refptr<webrtc::RTCStatsReport>)

} // namespace node_webrtc
