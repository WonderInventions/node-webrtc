#pragma once
#include <node-addon-api/napi.h>
#include <webrtc/p2p/base/transport_description.h>

namespace node_webrtc {

class RTCIceParameters : public Napi::ObjectWrap<RTCIceParameters> {
 public:                                   // ←── keep it here
  static void Init(Napi::Env, Napi::Object);

  explicit RTCIceParameters(const Napi::CallbackInfo&);

  /* expose native struct for converters */
  const cricket::IceParameters& Native() const { return _params; }

  /* ONE declaration only → delete any duplicate farther below */
  static Napi::FunctionReference _constructor;

 private:
  /* JS accessors */
  Napi::Value UsernameFragment(const Napi::CallbackInfo&);
  Napi::Value Password(const Napi::CallbackInfo&);
  Napi::Value IceLite(const Napi::CallbackInfo&);

  /* Stored state ------------------------------------------------------- */
  cricket::IceParameters _params;
  bool                   _iceLite = false;
};

}