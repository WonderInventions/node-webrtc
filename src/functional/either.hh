/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */

/*
 * This file defines Either, a sum type, modeled after the Haskell's:
 * https://hackage.haskell.org/package/base/docs/Data-Either.html
 */

#pragma once

#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace node_webrtc {

/**
 * Either is a sum type. It either contains a value of type L ("left") or a
 * value of type R ("right").
 * @tparam L the left type
 * @tparam R the right type
 */
template <typename L, typename R> class Either {
public:
  /**
   * Either forms an applicative. Apply an Either.
   * @tparam F the type of a function from R to S
   * @param f an Either of a function from R to S
   * @return the result of applying the Either
   */
  template <typename F>
  Either<L, typename std::invoke_result_t<F, R>>
  Apply(const Either<L, F> f) const {
    if (f.IsLeft()) {
      return Either<L, typename std::invoke_result_t<F, R>>::Left(
          f.UnsafeFromLeft());
    }
    if (IsLeft()) {
      return Either<L, typename std::invoke_result_t<F, R>>::Left(
          UnsafeFromLeft());
    }
    return Either<L, typename std::invoke_result_t<F, R>>::Left(
        UnsafeFromRight());
  }

  /**
   * Check whether or not the Either is "left".
   * @return true if the Either is left; otherwise, false
   */
  [[nodiscard]] bool IsLeft() const { return !_is_right; }

  /**
   * Check whether or not the Either is "right".
   * @return true if the Either is right; otherwise, false
   */
  [[nodiscard]] bool IsRight() const { return _is_right; }

  /**
   * Eliminate an Either. You must provide two functions for both the left and
   * right cases that convert each value to some common type T.
   * @tparam T the common type to convert to
   * @param fromLeft a function that converts values of type L to T
   * @param fromRight a function that converts values of type R to T
   * @return a value of type T
   */
  template <typename T>
  T FromEither(std::function<T(const L)> fromLeft,
               std::function<T(const R)> fromRight) const {
    return _is_right ? fromRight(UnsafeFromRight())
                     : fromLeft(UnsafeFromLeft());
  }

  /**
   * Eliminate an Either. You must provide a default value to handle the "right"
   * case.
   * @param default_value the default value to use in the right case
   * @return the value in the Either, if "left"; otherwise, the default value
   */
  L FromLeft(const L default_value) const {
    return _is_right ? default_value : UnsafeFromLeft();
  }

  /**
   * Eliminate an Either. You must provide a default value to handle the "left"
   * case.
   * @param default_value the default value to use in the left case
   * @return the value in the Either, if "right"; otherwise, the default value
   */
  R FromRight(const R default_value) const {
    return _is_right ? UnsafeFromRight() : default_value;
  }

  /**
   * Either forms a functor. Map a function over Either.
   * @tparam F the type of a function from R to S
   * @param f a function from R to S
   * @return the mapped Either
   */
  template <typename F>
  Either<L, typename std::invoke_result_t<F, R>> Map(F f) const {
    return _is_right ? Either<L, typename std::invoke_result_t<F, R>>::Right(
                           f(UnsafeFromRight()))
                     : Either<L, typename std::invoke_result_t<F, R>>::Left(
                           UnsafeFromLeft());
  }

  /**
   * Either forms an alternative. If "this" is "right", return this; otherwise,
   * that
   * @param that another Either
   * @return this or that
   */
  Either<L, R> Or(const Either<L, R> that) const {
    return _is_right ? this : that;
  }

  /**
   * Unsafely eliminate an Either. This only works if the Either is "left".
   * @return the left value in the Either, if left; otherwise, undefined
   */
  L UnsafeFromLeft() const {
    assert(!_is_right);
    return std::get<0>(_value);
  }

  /**
   * Unsafely eliminate an Either. This only works if the Either is "right".
   * @return the right value in the Either, if right; otherwise, undefined
   */
  R UnsafeFromRight() const {
    assert(_is_right);
    return std::get<1>(_value);
  }

  /**
   * Construct a "left" Either.
   * @param left the value to inject into the Either
   * @return a left Either
   */
  static Either<L, R> Left(const L left) { return Either(false, left); }

  /**
   * Construct a "right" Either.
   * @param right the value to inject into the Either
   * @return a right Either
   */
  static Either<L, R> Right(const R right) {
    return Either(true, std::variant<L, R>(std::in_place_index<1>, right));
  }

private:
  Either(bool is_right, const std::variant<L, R> value)
      : _is_right(is_right), _value(value) {}

  // Needed to know what index to access. It is entirely legal to make a
  // std::variant<int, int>, and in that case, the only way to get the second
  // int is to std::get<1>
  bool _is_right;
  std::variant<L, R> _value;
};

template <typename L, typename R> static Either<L, R> MakeRight(const R right) {
  return Either<L, R>::Right(right);
}

template <typename R, typename L> static Either<L, R> MakeLeft(const L left) {
  return Either<L, R>::Left(left);
}

} // namespace node_webrtc
