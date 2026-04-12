#include "src/dictionaries/webrtc/rtc_stats_member_interface.hh"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <node-addon-api/napi.h>
#include <webrtc/api/stats/attribute.h>

#include "src/converters.hh"

namespace node_webrtc {

TO_NAPI_IMPL(const webrtc::Attribute *, pair) {
  auto env = pair.first;
  auto value = pair.second;

  if (!value || !value->has_value()) {
    return Validation<Napi::Value>::Invalid("RTC stats attribute is undefined");
  }

  if (value->holds_alternative<bool>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<bool>()));
  }
  if (value->holds_alternative<int32_t>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<int32_t>()));
  }
  if (value->holds_alternative<uint32_t>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<uint32_t>()));
  }
  if (value->holds_alternative<int64_t>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<int64_t>()));
  }
  if (value->holds_alternative<uint64_t>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<uint64_t>()));
  }
  if (value->holds_alternative<double>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<double>()));
  }
  if (value->holds_alternative<std::string>()) {
    return From<Napi::Value>(std::make_pair(env, value->get<std::string>()));
  }
  if (value->holds_alternative<std::vector<bool>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<bool>>()));
  }
  if (value->holds_alternative<std::vector<int32_t>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<int32_t>>()));
  }
  if (value->holds_alternative<std::vector<uint32_t>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<uint32_t>>()));
  }
  if (value->holds_alternative<std::vector<int64_t>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<int64_t>>()));
  }
  if (value->holds_alternative<std::vector<uint64_t>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<uint64_t>>()));
  }
  if (value->holds_alternative<std::vector<double>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<double>>()));
  }
  if (value->holds_alternative<std::vector<std::string>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::vector<std::string>>()));
  }
  if (value->holds_alternative<std::map<std::string, uint64_t>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::map<std::string, uint64_t>>()));
  }
  if (value->holds_alternative<std::map<std::string, double>>()) {
    return From<Napi::Value>(
        std::make_pair(env, value->get<std::map<std::string, double>>()));
  }

  return Validation<Napi::Value>::Invalid(
      "RTC stats attribute type not supported, file a bug against "
      "node-webrtc");
}

} // namespace node_webrtc
