#pragma once

#include <webrtc/api/ice_transport_interface.h>  // for webrtc::IceParameters

namespace node_webrtc {

struct RTCIceParameters {
  RTCIceParameters()
    : params(webrtc::IceParameters()) {}
  explicit RTCIceParameters(const webrtc::IceParameters& p)
    : params(p) {}
  const webrtc::IceParameters params;
};

}  // namespace node_webrtc

// tell the macros what our fields are:
#define RTC_ICE_PARAMETERS RTCIceParameters
#define RTC_ICE_PARAMETERS_LIST                                           \
  DICT_DEFAULT(std::string, usernameFragment, "usernameFragment", "")      \
  DICT_DEFAULT(std::string, password,           "password",           "")  \
  DICT_OPTIONAL(bool,       iceLite,            "iceLite")

#define DICT(X) RTC_ICE_PARAMETERS##X
#include "src/dictionaries/macros/decls.hh"
#undef DICT
