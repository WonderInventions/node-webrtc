#pragma once

// libwebrtc main moved large parts of the former rtc::* API surface into the
// webrtc namespace. Keep the existing addon sources compiling while we port the
// remaining API deltas incrementally.
namespace webrtc {}
namespace rtc = webrtc;
