#include "src/dictionaries/webrtc/rtp_header_extension_parameters.hh"

#include <utility>

#include <node-addon-api/napi.h>
#include <webrtc/api/rtp_parameters.h>

#include "src/converters/object.hh"
#include "src/dictionaries/macros/napi.hh"
#include "src/functional/curry.hh"
#include "src/functional/operators.hh"
#include "src/functional/validation.hh"

namespace node_webrtc {

TO_NAPI_IMPL(webrtc::RtpHeaderExtensionParameters, pair) {
  auto env = pair.first;
  Napi::EscapableHandleScope scope(env);
  auto params = pair.second;
  NODE_WEBRTC_CREATE_OBJECT_OR_RETURN(env, object)
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "uri", params.uri)
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "id", params.id)
  NODE_WEBRTC_CONVERT_AND_SET_OR_RETURN(env, object, "encrypted",
                                        params.encrypt)
  return Pure(scope.Escape(object));
}

static webrtc::RtpHeaderExtensionParameters
NapiToRtpHeaderExtensionParameters(std::string const &uri, int id,
                                   bool encrypted) {
  webrtc::RtpHeaderExtensionParameters parameters;
  parameters.uri = uri;
  parameters.id = id;
  parameters.encrypt = encrypted;
  return parameters;
}

FROM_NAPI_IMPL(webrtc::RtpHeaderExtensionParameters, value) {
  return From<Napi::Object>(value)
      .FlatMap<webrtc::RtpHeaderExtensionParameters>([](auto object) {
        return curry(NapiToRtpHeaderExtensionParameters) %
               GetRequired<std::string>(object, "uri") *
               GetRequired<uint8_t>(object, "id") *
               GetOptional<bool>(object, "encrypted", false);
      });
}

} // namespace node_webrtc
