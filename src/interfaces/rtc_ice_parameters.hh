#pragma once

#include <node-addon-api/napi.h>
#include <webrtc/api/ice_transport_interface.h>

namespace node_webrtc {

/**
 * JS‑visible wrapper around webrtc::IceParameters
 *   new RTCIceParameters({ usernameFragment, password, iceLite })
 */
class RTCIceParameters
  : public Napi::ObjectWrap<RTCIceParameters> {

 public:
  /* Called from binding.cc */
  static void Init(Napi::Env, Napi::Object exports);

  /* JS constructor */
  explicit RTCIceParameters(const Napi::CallbackInfo& info);

  /* C++ getter for native code that needs the struct */
  const webrtc::IceParameters& native() const { return _params; }

 private:
  /* JS getters */
  Napi::Value UsernameFragment(const Napi::CallbackInfo& info);
  Napi::Value Password(const Napi::CallbackInfo& info);
  Napi::Value IceLite(const Napi::CallbackInfo& info);

  /* Internal state */
  webrtc::IceParameters _params;

  /* Cached JS constructor */
  static Napi::FunctionReference _constructor;
};

}  // namespace node_webrtc
