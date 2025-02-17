#pragma once

#include <iosfwd>
#include <string>

// IWYU pragma: no_forward_declare node_webrtc::RTCDtlsFingerprint
// IWYU pragma: no_include "src/dictionaries/macros/impls.hh"

#define RTC_DTLS_FINGERPRINT RTCDtlsFingerprint
#define RTC_DTLS_FINGERPRINT_LIST                                              \
  DICT_OPTIONAL(std::string, algorithm, "algorithm")                           \
  DICT_OPTIONAL(std::string, value, "value")

#define DICT(X) RTC_DTLS_FINGERPRINT##X
#include "src/dictionaries/macros/def.hh"
// ordering
#include "src/dictionaries/macros/decls.hh"
#undef DICT
