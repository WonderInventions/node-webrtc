#include "src/dictionaries/node_webrtc/rtc_ice_parameters.hh"
#include "src/functional/maybe.hh"
#include "src/functional/validation.hh"

namespace node_webrtc {

#define RTC_ICE_PARAMETERS_FN CreateRTCIceParameters

static Validation<RTCIceParameters>
RTC_ICE_PARAMETERS_FN(const std::string& usernameFragment,
                      const std::string& password,
                      const Maybe<bool> iceLite) {
  webrtc::IceParameters p;
  p.username_fragment = usernameFragment;
  p.password          = password;
  p.lite              = iceLite.FromMaybe(false);
  return Pure(RTCIceParameters(p));
}

}  // namespace node_webrtc

#define DICT(X) RTC_ICE_PARAMETERS##X
#include "src/dictionaries/macros/impls.hh"
#undef DICT
