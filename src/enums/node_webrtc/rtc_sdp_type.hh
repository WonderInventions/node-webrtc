#pragma once

// IWYU pragma: no_include "src/enums/macros/impls.hh"

#define RTC_SDP_TYPE RTCSdpType
#define RTC_SDP_TYPE_NAME "RTCSdpType"
#define RTC_SDP_TYPE_LIST                                                      \
  ENUM_SUPPORTED(kOffer, "offer")                                              \
  ENUM_SUPPORTED(kAnswer, "answer")                                            \
  ENUM_SUPPORTED(kPrAnswer, "pranswer")                                        \
  ENUM_SUPPORTED(kRollback, "rollback")

#define ENUM(X) RTC_SDP_TYPE##X
#include "src/enums/macros/def.hh"
// ordering
#include "src/enums/macros/decls.hh"
#undef ENUM
