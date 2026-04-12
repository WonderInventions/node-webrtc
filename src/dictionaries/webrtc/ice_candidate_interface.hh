#pragma once

#include <memory>

#include <webrtc/api/jsep.h>

#include "src/converters/napi.hh"

#define ICE_CANDIDATE_INTERFACE webrtc::IceCandidateInterface *

#define DICT(X) ICE_CANDIDATE_INTERFACE##X
#include "src/dictionaries/macros/decls.hh"
#undef DICT

namespace node_webrtc {

DECLARE_FROM_NAPI(std::shared_ptr<webrtc::IceCandidateInterface>)

} // namespace node_webrtc
