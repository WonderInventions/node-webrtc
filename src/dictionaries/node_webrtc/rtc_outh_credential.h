#pragma once

#include <iosfwd>
#include <string>

// IWYU pragma: no_forward_declare node_webrtc::RTCOAuthCredential
// IWYU pragma: no_include "src/dictionaries/macros/impls.h"

#define RTC_OAUTH_CREDENTIAL RTCOAuthCredential
#define RTC_OAUTH_CREDENTIAL_LIST                                              \
  DICT_REQUIRED(std::string, macKey, "macKey")                                 \
  DICT_REQUIRED(std::string, accessToken, "accessToken")

#define DICT(X) RTC_OAUTH_CREDENTIAL##X
#include "src/dictionaries/macros/decls.h"
#include "src/dictionaries/macros/def.h"
#undef DICT
