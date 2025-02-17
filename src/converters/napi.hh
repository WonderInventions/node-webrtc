#pragma once

#include <cstdint>
#include <iosfwd>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <node-addon-api/napi.h>

#include "src/converters.hh"
#include "src/converters/macros.hh"
#include "src/functional/maybe.hh"
#include "src/functional/validation.hh"

#define CONVERT_OR_THROW_AND_RETURN_NAPI(E, I, O, T)                           \
  auto NODE_WEBRTC_UNIQUE_NAME(validation) =                                   \
      From<detail::argument_type<void(T)>::type>(std::make_pair(E, I));        \
  if (NODE_WEBRTC_UNIQUE_NAME(validation).IsInvalid()) {                       \
    auto error = NODE_WEBRTC_UNIQUE_NAME(validation).ToErrors()[0];            \
    Napi::TypeError::New(E, error).ThrowAsJavaScriptException();               \
    return E.Undefined();                                                      \
  }                                                                            \
  auto O = NODE_WEBRTC_UNIQUE_NAME(validation).UnsafeFromValid();

#define CONVERT_OR_THROW_AND_RETURN_VOID_NAPI(E, I, O, T)                      \
  auto NODE_WEBRTC_UNIQUE_NAME(validation) =                                   \
      From<detail::argument_type<void(T)>::type>(std::make_pair(E, I));        \
  if (NODE_WEBRTC_UNIQUE_NAME(validation).IsInvalid()) {                       \
    auto error = NODE_WEBRTC_UNIQUE_NAME(validation).ToErrors()[0];            \
    Napi::TypeError::New(E, error).ThrowAsJavaScriptException();               \
    return;                                                                    \
  }                                                                            \
  auto O = NODE_WEBRTC_UNIQUE_NAME(validation).UnsafeFromValid();

#define CONVERT_OR_REJECT_AND_RETURN_NAPI(D, I, O, T)                          \
  auto NODE_WEBRTC_UNIQUE_NAME(validation) =                                   \
      From<detail::argument_type<void(T)>::type>(std::make_pair(D.Env(), I));  \
  if (NODE_WEBRTC_UNIQUE_NAME(validation).IsInvalid()) {                       \
    auto error = NODE_WEBRTC_UNIQUE_NAME(validation).ToErrors()[0];            \
    D.Reject(Napi::TypeError::New(D.Env(), error).Value());                    \
    return D.Env().Undefined();                                                \
  }                                                                            \
  auto O = NODE_WEBRTC_UNIQUE_NAME(validation).UnsafeFromValid();

namespace node_webrtc {

#define DECLARE_TO_NAPI(T)                                                     \
  DECLARE_CONVERTER(std::pair<Napi::Env COMMA T>, Napi::Value)

#define DECLARE_FROM_NAPI(T) DECLARE_CONVERTER(Napi::Value, T)

#define DECLARE_TO_AND_FROM_NAPI(T)                                            \
  DECLARE_TO_NAPI(T)                                                           \
  DECLARE_FROM_NAPI(T)

#define TO_NAPI_IMPL(T, V)                                                     \
  CONVERTER_IMPL(std::pair<Napi::Env COMMA T>, Napi::Value, V)

#define FROM_NAPI_IMPL(T, V) CONVERTER_IMPL(Napi::Value, T, V)

DECLARE_TO_AND_FROM_NAPI(bool)
DECLARE_TO_AND_FROM_NAPI(double)
DECLARE_TO_AND_FROM_NAPI(uint8_t)
DECLARE_TO_AND_FROM_NAPI(uint16_t)
DECLARE_TO_AND_FROM_NAPI(uint32_t)
DECLARE_TO_AND_FROM_NAPI(uint64_t)
DECLARE_TO_AND_FROM_NAPI(int8_t)
DECLARE_TO_AND_FROM_NAPI(int16_t)
DECLARE_TO_AND_FROM_NAPI(int32_t)
DECLARE_TO_AND_FROM_NAPI(int64_t)
DECLARE_TO_AND_FROM_NAPI(std::string)
DECLARE_TO_NAPI(Napi::Error)
DECLARE_FROM_NAPI(Napi::Function)
DECLARE_FROM_NAPI(Napi::Object)
DECLARE_TO_NAPI(Napi::Value)
DECLARE_TO_NAPI(std::vector<bool>)
DECLARE_FROM_NAPI(Napi::ArrayBuffer)

template <typename T> struct Converter<Napi::Value, Maybe<T>> {
  static Validation<Maybe<T>> Convert(const Napi::Value value) {
    return value.IsUndefined() ? Pure(MakeNothing<T>())
                               : From<T>(value).Map(&MakeJust<T>);
  }
};

template <typename T>
struct Converter<std::pair<Napi::Env, Maybe<T>>, Napi::Value> {
  static Validation<Napi::Value>
  Convert(const std::pair<Napi::Env, Maybe<T>> pair) {
    return pair.second.IsJust() ? From<Napi::Value>(std::make_pair(
                                      pair.first, pair.second.UnsafeFromJust()))
                                : Pure(pair.first.Null());
  }
};

template <> struct Converter<Napi::Value, Napi::Array> {
  static Validation<Napi::Array> Convert(Napi::Value value) {
    return value.IsArray()
               ? Pure(value.As<Napi::Array>())
               : Validation<Napi::Array>::Invalid(("Expected an array"));
  }
};

template <typename T> struct Converter<Napi::Array, std::vector<T>> {
  static Validation<std::vector<T>> Convert(const Napi::Array array) {
    auto validated = std::vector<T>();
    validated.reserve(array.Length());
    for (uint32_t i = 0; i < array.Length(); i++) {
      auto maybeValue = array.Get(i);
      if (maybeValue.Env().IsExceptionPending()) {
        return Validation<std::vector<T>>::Invalid(
            maybeValue.Env().GetAndClearPendingException().Message());
      }
      auto maybeValidated = From<T>(maybeValue);
      if (maybeValidated.IsInvalid()) {
        return Validation<std::vector<T>>::Invalid(maybeValidated.ToErrors());
      }
      validated.push_back(maybeValidated.UnsafeFromValid());
    }
    return Pure(validated);
  }
};

template <typename T> struct Converter<Napi::Value, std::vector<T>> {
  static Validation<std::vector<T>> Convert(const Napi::Value value) {
    std::cout << "Doing Napi::Value -> vector conversion\n";
    return Converter<Napi::Value, Napi::Array>::Convert(value)
        .FlatMap<std::vector<T>>(
            Converter<Napi::Array, std::vector<T>>::Convert);
  }
};

template <typename T>
struct Converter<std::pair<Napi::Env, std::vector<T>>, Napi::Value> {
  static Validation<Napi::Value>
  Convert(std::pair<Napi::Env, std::vector<T>> pair) {
    std::cout << "Doing vector -> Napi::Value conversion\n";
    auto env = pair.first;
    Napi::EscapableHandleScope scope(env);
    auto values = pair.second;
    auto maybeArray = Napi::Array::New(env, values.size());
    std::cout << "Doing vector -> Napi::Value conversion check 1\n";
    if (maybeArray.Env().IsExceptionPending()) {
      std::cout << "Vector conversion: bad 1\n";
      return Validation<Napi::Value>::Invalid(
          maybeArray.Env().GetAndClearPendingException().Message());
    }
    std::cout << "Doing vector -> Napi::Value conversion check 2\n";
    uint32_t i = 0;
    for (const auto &value : values) {
      auto maybeValue = From<Napi::Value>(std::make_pair(env, value));
      std::cout << "Doing vector -> Napi::Value conversion check 3\n";
      std::cout << "Doing vector -> Napi::Value conversion check 3.1\n";
      if (maybeValue.IsInvalid()) {
        std::cout << "Vector conversion: bad 2\n";
        return Validation<Napi::Value>::Invalid(maybeValue.ToErrors());
      }
      std::cout << "Doing vector -> Napi::Value conversion check 3.2\n";
      Napi::Value realValue = maybeValue.UnsafeFromValid();
      std::cout
          << "Doing vector -> Napi::Value conversion check 3.3: IsEmpty(): "
          << realValue.IsEmpty() << "\n";
      maybeArray.Set(i++, realValue);
      std::cout << "Doing vector -> Napi::Value conversion check 4\n";
      if (maybeArray.Env().IsExceptionPending()) {
        std::cout << "Vector conversion: bad 3\n";
        return Validation<Napi::Value>::Invalid(
            maybeArray.Env().GetAndClearPendingException().Message());
      }
    }

    std::cout << "Doing vector -> Napi::Value conversion check 5\n";
    return Pure(scope.Escape(maybeArray));
  }
};

template <typename T> struct Converter<Napi::Object, std::map<std::string, T>> {
  using Result = Validation<std::map<std::string, T>>;
  static Result Convert(const Napi::Object o) {
    auto validated = std::map<std::string, T>();
    auto propertyNames = o.GetPropertyNames();
    if (propertyNames.Env().IsExceptionPending()) {
      return Result::Invalid(
          propertyNames.Env().GetAndClearPendingException().Message());
    }
    for (uint32_t i = 0; i < propertyNames.Length(); i++) {
      auto key = propertyNames.Get(i);
      if (key.Env().IsExceptionPending()) {
        return Result::Invalid(
            key.Env().GetAndClearPendingException().Message());
      }
      if (!o.HasOwnProperty(key)) {
        continue;
      }
      auto maybeKey = From<std::string>(key);
      if (maybeKey.IsInvalid()) {
        return Result::Invalid(maybeKey.ToErrors());
      }
      auto value = o.Get(key);
      if (value.Env().IsExceptionPending()) {
        return Result::Invalid(
            value.Env().GetAndClearPendingException().Message());
      }
      auto maybeValue = From<T>(value);
      if (maybeValue.IsInvalid()) {
        return Result::Invalid(maybeValue.ToErrors());
      }

      validated[maybeKey.UnsafeFromValid()] = maybeValue.UnsafeFromValid();
    }
    return Pure(validated);
  }
};

template <typename T> struct Converter<Napi::Value, std::map<std::string, T>> {
  static Validation<std::map<std::string, T>> Convert(const Napi::Value value) {
    return Converter<Napi::Value, Napi::Object>::Convert(value)
        .FlatMap<std::map<std::string, T>>(
            Converter<Napi::Object, std::map<std::string, T>>::Convert);
  }
};

template <typename T>
struct Converter<std::pair<Napi::Env, std::map<std::string, T>>, Napi::Value> {
  static Validation<Napi::Value>
  Convert(std::pair<Napi::Env, std::map<std::string, T>> pair) {
    auto env = pair.first;
    Napi::EscapableHandleScope scope(env);
    auto values = pair.second;
    auto maybeObject = Napi::Object::New(env);
    if (maybeObject.Env().IsExceptionPending()) {
      return Validation<Napi::Value>::Invalid(
          maybeObject.Env().GetAndClearPendingException().Message());
    }
    for (auto it = values.begin(); it != values.end(); ++it) {
      auto key = it->first;
      auto value = it->second;
      auto maybeValue = From<Napi::Value>(std::make_pair(env, value));
      if (maybeValue.IsInvalid()) {
        return Validation<Napi::Value>::Invalid(maybeValue.ToErrors());
      }
      maybeObject.Set(key, maybeValue.UnsafeFromValid());
      if (maybeObject.Env().IsExceptionPending()) {
        return Validation<Napi::Value>::Invalid(
            maybeObject.Env().GetAndClearPendingException().Message());
      }
    }
    return Pure(scope.Escape(maybeObject));
  }
};

template <typename T> struct Converter<Napi::Value, Napi::External<T>> {
  static Validation<Napi::External<T>> Convert(Napi::Value value) {
    return value.IsExternal()
               ? Pure(value.As<Napi::External<T>>())
               : Validation<Napi::External<T>>::Invalid("Expected an external");
  }
};

} // namespace node_webrtc
