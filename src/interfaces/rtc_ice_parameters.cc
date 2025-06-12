#include "src/interfaces/rtc_ice_parameters.hh"

#include "src/converters/arguments.hh" // Argument helpers/macros (same as other interfaces)
#include "src/node/error_factory.hh" // For consistent error handling

using namespace node_webrtc;

Napi::FunctionReference RTCIceParameters::_constructor;

/* static */
void RTCIceParameters::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function ctor =
      DefineClass(env, "RTCIceParameters",
                  {
                      InstanceAccessor("usernameFragment",
                                       &RTCIceParameters::UsernameFragment,
                                       nullptr, napi_enumerable),
                      InstanceAccessor("password", &RTCIceParameters::Password,
                                       nullptr, napi_enumerable),
                      InstanceAccessor("iceLite", &RTCIceParameters::IceLite,
                                       nullptr, napi_enumerable),
                  });

  _constructor = Napi::Persistent(ctor);
  _constructor.SuppressDestruct();

  exports.Set("RTCIceParameters", ctor);
}

/* JS constructor */
RTCIceParameters::RTCIceParameters(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<RTCIceParameters>(info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "RTCIceParameters expects a single object")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::Object dict = info[0].As<Napi::Object>();

  /* Pull & validate the fields */
  if (!dict.Has("usernameFragment") || !dict.Has("password")) {
    Napi::TypeError::New(env, "usernameFragment and password are required")
        .ThrowAsJavaScriptException();
    return;
  }

  _params.ufrag = dict.Get("usernameFragment").As<Napi::String>().Utf8Value();
  _params.pwd = dict.Get("password").As<Napi::String>().Utf8Value();
  _iceLite = dict.Has("iceLite")
                 ? dict.Get("iceLite").As<Napi::Boolean>().Value()
                 : false;
}

/* JS getters */
Napi::Value RTCIceParameters::UsernameFragment(const Napi::CallbackInfo &info) {
  return Napi::String::New(info.Env(), _params.ufrag);
}

Napi::Value RTCIceParameters::Password(const Napi::CallbackInfo &info) {
  return Napi::String::New(info.Env(), _params.pwd);
}

Napi::Value RTCIceParameters::IceLite(const Napi::CallbackInfo &info) {
  return Napi::Boolean::New(info.Env(), _iceLite);
}
