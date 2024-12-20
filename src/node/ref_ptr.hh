/* Copyright (c) 2024 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

namespace node_webrtc {

/**
 * A class that provides strong ownership of a Napi::ObjectWrap pointer.
 *
 * The underlying value is `Ref()`ed when this is created, and `Unref()`ed when
 * this is destroyed.
 */
template <typename T> class RefPtr {
public:
  explicit RefPtr(T *ptr) : _ptr(ptr) { _ptr->Ref(); }
  // NOTE(jack): I'd like it to be impossible to make an invalid one of these.
  // Unfortunately, due to Reasons, we must provide a default constructor.
  RefPtr() : _ptr(nullptr) {}

  // Allow assignment from a raw pointer, as an alternate constructor
  RefPtr &operator=(T *ptr) {
    _ptr = ptr;
    _ptr->Ref();
    return *this;
  }
  // Allow implicit conversion to a raw pointer
  operator T *() const { return _ptr; }

  // Allow access to inner fields thru the pointer
  T *operator->() { return _ptr; }
  ~RefPtr() {
    if (_ptr) {
      _ptr->Unref();
    }
  }

  // Copy constructor: add another ref
  RefPtr(const RefPtr &other) : _ptr(other._ptr) { _ptr->Ref(); }
  RefPtr &operator=(const RefPtr &other) {
    if (this == &other) {
      return *this;
    }

    if (_ptr) {
      _ptr->Unref();
    }
    _ptr = other._ptr;
    _ptr->Ref();
    return *this;
  }

  // Move constructor: set other's _ptr to nullptr in order to transfer
  // ownership
  RefPtr(RefPtr &&other) noexcept : _ptr(other._ptr) { other._ptr = nullptr; }
  RefPtr &operator=(RefPtr &&other) noexcept {
    if (_ptr) {
      _ptr->Unref();
    }
    _ptr = other._ptr;
    other._ptr = nullptr;
    _ptr->Ref();
    return *this;
  }

private:
  T *_ptr;
};

} // namespace node_webrtc
