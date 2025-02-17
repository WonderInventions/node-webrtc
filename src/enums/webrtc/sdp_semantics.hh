#pragma once

#include <webrtc/api/peer_connection_interface.h>

// IWYU pragma: no_include "src/enums/macros/impls.hh"

#define SDP_SEMANTICS webrtc::SdpSemantics
#define SDP_SEMANTICS_NAME "RTCSdpSemantics"
#define SDP_SEMANTICS_LIST                                                     \
  ENUM_SUPPORTED(SDP_SEMANTICS::kPlanB, "plan-b")                              \
  ENUM_SUPPORTED(SDP_SEMANTICS::kUnifiedPlan, "unified-plan")

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define ENUM(X) SDP_SEMANTICS##X
#include "src/enums/macros/decls.hh"
#undef ENUM

#pragma clang diagnostic pop
