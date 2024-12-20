/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include "src/utilities/bidi_map.hh"

namespace node_webrtc {

/**
 * Class that provides weak ownership of types `T`, caching based on a key `U`.
 */
template <typename T, typename U, typename... V> class Wrap {
public:
  Wrap(Wrap const &) = delete;
  Wrap(Wrap &&) = delete;
  Wrap &operator=(Wrap const &) = delete;
  Wrap &operator=(Wrap &&) = delete;
  Wrap() = delete;
  explicit Wrap(T (*create)(V..., U)) : _create(create) {}
  ~Wrap() = default;

  T GetOrCreate(V... args, U key) {
    return _map.computeIfAbsent(key, [this, key, args...]() {
      auto out = _create(args..., key);
      return out;
    });
  }

  T Get(U key) { return _map.get(key).FromMaybe(nullptr); }

  void Release(T value) { _map.reverseRemove(value); }

private:
  T (*_create)(V..., U);
  BidiMap<U, T> _map;
};

} // namespace node_webrtc
