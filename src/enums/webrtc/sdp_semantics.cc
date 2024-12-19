#include "src/enums/webrtc/sdp_semantics.hh"

#include <webrtc/api/peer_connection_interface.h> // IWYU pragma: keep

#define ENUM(X) SDP_SEMANTICS##X
#include "src/enums/macros/impls.hh"
#undef ENUM
