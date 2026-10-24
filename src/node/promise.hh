/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <memory>
#include <type_traits>

#include <node-addon-api/napi.h>

#include "events.hh"
#include "utility.hh"

namespace node_webrtc {

template <typename T, typename F> class Promise : public Event<T> {
public:
  Promise(Napi::Promise::Deferred &deferred, F callback,
          bool dispatch_after_stop = false)
      : _callback(callback), _deferred(deferred),
        _dispatch_after_stop(dispatch_after_stop) {}

  void Dispatch(T &) override { _callback(_deferred); }

  bool DispatchAfterStop() const override { return _dispatch_after_stop; }

private:
  F _callback;
  Napi::Promise::Deferred _deferred;
  bool _dispatch_after_stop;
};

template <typename T, typename F>
std::unique_ptr<Promise<T, F>> CreatePromise(Napi::Promise::Deferred deferred,
                                             F callback) {
  return std::make_unique<Promise<T, F>>(deferred, callback);
}

template <typename T> class PromiseCreator {
public:
  /**
   * @param dispatch_after_stop settle the Promise if its result was queued
   * before the target's EventLoop stopped (see Event::DispatchAfterStop)
   */
  PromiseCreator(T *target, Napi::Promise::Deferred deferred,
                 bool dispatch_after_stop = false)
      : _target(target), _deferred(deferred),
        _dispatch_after_stop(dispatch_after_stop) {}

  template <typename F> void Dispatch(F callback) {
    _target->Dispatch(std::make_unique<Promise<T, F>>(_deferred, callback,
                                                      _dispatch_after_stop));
  }

  template <typename F> void Resolve(F value) {
    Dispatch([value](auto deferred) { node_webrtc::Resolve(deferred, value); });
  }

  template <typename F> void Reject(F value) {
    Dispatch([value](auto deferred) { node_webrtc::Reject(deferred, value); });
  }

private:
  T *_target;
  Napi::Promise::Deferred _deferred;
  bool _dispatch_after_stop;
};

} // namespace node_webrtc
