/* Copyright (c) 2024 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <cstddef>
#include <napi.h>

namespace node_webrtc {

/**
 * A class that provides strong ownership of a Napi::ObjectWrap pointer.
 *
 * The RefPtr keeps the wrapped JavaScript object alive through its *own*
 * persistent reference (`napi_create_reference`) rather than by calling
 * `Ref()`/`Unref()` on the ObjectWrap it points at.
 *
 * When the Node.js environment shuts down, remaining wrappers are finalized in
 * an unspecified order (most recently created first in practice). If this
 * destructor ran after the pointee's wrapper had already been deleted, calling
 * `_ptr->Unref()` would dereference freed memory: `~RTCRtpReceiver` unref'ing a
 * `MediaStreamTrack` wrapper that Node had just finalized crashed with SIGSEGV
 * in `napi_reference_unref`. A reference we own ourselves is userland-owned,
 * survives environment finalization, and deleting it is always valid.
 *
 * We don't provide a `WeakRefPtr` class, because that is effectively just
 * a regular pointer type.
 */
template <typename T> class RefPtr {
public:
  explicit RefPtr(T *ptr) : _ptr(ptr) { acquire(); }
  // NOTE(jack): I'd like it to be impossible to make an invalid one of these.
  // Unfortunately, due to Reasons, we must provide a default constructor.
  RefPtr() : _ptr(nullptr) {}
  // ... and a nullptr_t constructor
  RefPtr(std::nullptr_t) : _ptr(nullptr) {}

  ~RefPtr() { release(); }

  // Allow assignment from a raw pointer, as an alternate copy constructor
  RefPtr &operator=(T *ptr) {
    if (_ptr == ptr) {
      return *this;
    }
    release();
    _ptr = ptr;
    acquire();
    return *this;
  }

  // Allow implicit and explicit conversion to a raw pointer
  operator T *() const { return _ptr; }
  T *ptr() const { return _ptr; }

  // Allow access to inner fields thru the pointer
  T *operator->() { return _ptr; }

  // Copy constructor: take another reference of our own
  RefPtr(const RefPtr &other) : _ptr(other._ptr) { acquire(); }
  RefPtr &operator=(const RefPtr &other) {
    if (this == &other) {
      return *this;
    }
    release();
    _ptr = other._ptr;
    acquire();
    return *this;
  }

  // Move constructor: transfer the pointer and the reference we already hold
  RefPtr(RefPtr &&other) noexcept
      : _ptr(other._ptr), _ref(std::move(other._ref)) {
    other._ptr = nullptr;
  }
  RefPtr &operator=(RefPtr &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    release();
    _ptr = other._ptr;
    _ref = std::move(other._ref);
    other._ptr = nullptr;
    return *this;
  }

private:
  void acquire() {
    if (!_ptr) {
      return;
    }
    // ObjectWrap<T> is a Reference<Object> to its own JavaScript object. If
    // that object is already unreachable (collected, finalizer pending) there
    // is nothing left to pin; keep the raw pointer, as before, but hold no
    // reference of our own.
    Napi::Object value = _ptr->Value();
    if (value.IsEmpty()) {
      return;
    }
    _ref = Napi::Persistent(value);
  }

  void release() {
    // Never touches `*_ptr`: safe even if the pointee's wrapper was already
    // finalized by the environment shutdown.
    _ref.Reset();
    _ptr = nullptr;
  }

  T *_ptr;
  Napi::ObjectReference _ref;
};

} // namespace node_webrtc
