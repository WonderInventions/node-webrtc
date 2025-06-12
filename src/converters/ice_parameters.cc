#include "src/converters/ice_parameters.hh"
#include "src/functional/validation.hh"
#include "src/interfaces/rtc_ice_parameters.hh"

namespace node_webrtc {

FROM_NAPI_IMPL(cricket::IceParameters, value) {
  auto env = value.Env();

  if (!value.IsObject()) {
    return Validation<cricket::IceParameters>::Invalid(
        "RTCIceParameters: expected an object");
  }
  auto obj = value.As<Napi::Object>();

  if (obj.InstanceOf(
          RTCIceParameters::_constructor.Value().As<Napi::Function>())) {
    auto *wrap = Napi::ObjectWrap<RTCIceParameters>::Unwrap(obj);
    return Pure(wrap->Native());
  }

  if (!obj.Has("usernameFragment") || !obj.Has("password")) {
    return Validation<cricket::IceParameters>::Invalid(
        "RTCIceParameters: usernameFragment and password are required");
  }

  cricket::IceParameters p;
  p.ufrag = obj.Get("usernameFragment").As<Napi::String>().Utf8Value();
  p.pwd = obj.Get("password").As<Napi::String>().Utf8Value();

  return Pure(p);
}

TO_NAPI_IMPL(cricket::IceParameters, pair) {
  Napi::Env env = pair.first;
  const auto &p = pair.second;

  Napi::EscapableHandleScope scope(env);
  Napi::Object obj = Napi::Object::New(env);

  obj.Set("usernameFragment", Napi::String::New(env, p.ufrag));
  obj.Set("password", Napi::String::New(env, p.pwd));
  /* We don’t know the lite flag here (native struct lacks it) → default false
   */
  obj.Set("iceLite", Napi::Boolean::New(env, false));

  return Pure(scope.Escape(obj));
}

} // namespace node_webrtc
