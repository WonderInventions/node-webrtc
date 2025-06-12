#pragma once
#include <node-addon-api/napi.h>
#include <webrtc/p2p/base/transport_description.h>

namespace node_webrtc {

class RTCIceParameters : public Napi::ObjectWrap<RTCIceParameters> {
 public:
  static void Init(Napi::Env, Napi::Object);

  explicit RTCIceParameters(const Napi::CallbackInfo&);

  const cricket::IceParameters& Native() const { return _params; }

  static Napi::FunctionReference _constructor;

 private:
  Napi::Value UsernameFragment(const Napi::CallbackInfo&);
  Napi::Value Password(const Napi::CallbackInfo&);
  Napi::Value IceLite(const Napi::CallbackInfo&);

  /* Stored state ------------------------------------------------------- */
  cricket::IceParameters _params;
  bool                   _iceLite = false;
};

}
