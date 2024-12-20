#include "src/enums/webrtc/sdp_semantics.hh"

#include <webrtc/api/peer_connection_interface.h> // IWYU pragma: keep

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define ENUM(X) SDP_SEMANTICS##X
#include "src/enums/macros/impls.hh"
#undef ENUM

#pragma clang diagnostic pop
