#include "src/converters/ice_parameters.hh"
#include "src/functional/validation.hh"

namespace node_webrtc {

// ---------- FROM NAPI (JS → native) ----------
FROM_NAPI_IMPL(webrtc::IceParameters, value) {
  auto env = value.Env();

  // Allow either a plain object literal or an RTCIceParameters instance
  if (!value.IsObject()) {
    return Validation<webrtc::IceParameters>::Invalid(
        "RTCIceParameters: expected an object");
  }

  auto obj = value.As<Napi::Object>();

  // Case 1: wrapper instance
  if (RTCIceParameters::_constructor.Value().InstanceOf(obj)) {
    auto* wrap = Napi::ObjectWrap<RTCIceParameters>::Unwrap(obj);
    return Pure(wrap->native());
  }

  // Case 2: bare dictionary { usernameFragment, password, iceLite }
  if (!obj.Has("usernameFragment") || !obj.Has("password")) {
    return Validation<webrtc::IceParameters>::Invalid(
        "RTCIceParameters: usernameFragment and password are required");
  }

  webrtc::IceParameters p;
  p.username_fragment = obj.Get("usernameFragment")
                            .As<Napi::String>()
                            .Utf8Value();
  p.password          = obj.Get("password")
                            .As<Napi::String>()
                            .Utf8Value();
  p.lite = obj.Has("iceLite")
             ? obj.Get("iceLite").As<Napi::Boolean>().Value()
             : false;

  return Pure(p);
}

// ---------- TO NAPI (native → JS) ----------
TO_NAPI_IMPL(webrtc::IceParameters, pair) {
  auto env = pair.first;
  const auto& p = pair.second;

  Napi::EscapableHandleScope scope(env);
  auto obj = Napi::Object::New(env);
  obj.Set("usernameFragment", Napi::String::New(env, p.username_fragment));
  obj.Set("password",         Napi::String::New(env, p.password));
  obj.Set("iceLite",          Napi::Boolean::New(env, p.lite));

  return Pure(scope.Escape(obj));
}

}  // namespace node_webrtc
