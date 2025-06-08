#pragma once

#include <webrtc/api/peer_connection_interface.h>

#include "src/converters/napi.hh"
#include "src/dictionaries/node_webrtc/unsigned_short_range.hh"

namespace node_webrtc {

struct ExtendedRTCConfiguration {
  ExtendedRTCConfiguration() : portRange(UnsignedShortRange()) {}

  ExtendedRTCConfiguration(
      const webrtc::PeerConnectionInterface::RTCConfiguration &configuration,
      const UnsignedShortRange portRange,
      bool iceLite = false)
      : configuration(configuration), portRange(portRange), iceLite(iceLite) {}

  webrtc::PeerConnectionInterface::RTCConfiguration configuration{};
  UnsignedShortRange portRange;
  bool iceLite = false;
};

DECLARE_TO_AND_FROM_NAPI(ExtendedRTCConfiguration)

} // namespace node_webrtc
