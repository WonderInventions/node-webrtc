/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <mutex>

#include <node-addon-api/napi.h>
#include <webrtc/api/scoped_refptr.h>
#include <webrtc/api/sctp_transport_interface.h>

#include "src/interfaces/rtc_dtls_transport.hh"
#include "src/node/async_object_wrap_with_loop.hh"
#include "src/node/ref_ptr.hh"
#include "src/node/wrap.hh"

namespace node_webrtc {

class PeerConnectionFactory;

class RTCSctpTransport : public AsyncObjectWrapWithLoop<RTCSctpTransport>,
                         public webrtc::SctpTransportObserverInterface {
public:
  explicit RTCSctpTransport(const Napi::CallbackInfo &);
  ~RTCSctpTransport() override;

  RTCSctpTransport(const RTCSctpTransport &) = delete;
  RTCSctpTransport(RTCSctpTransport &&) = delete;
  RTCSctpTransport &operator=(const RTCSctpTransport &) = delete;
  RTCSctpTransport &operator=(RTCSctpTransport &&) = delete;

  static void Init(Napi::Env, Napi::Object);

  static ::node_webrtc::Wrap<RTCSctpTransport *,
                             rtc::scoped_refptr<webrtc::SctpTransportInterface>,
                             PeerConnectionFactory *> *
  wrap();

  void OnStateChange(webrtc::SctpTransportInformation) override;

protected:
  void Stop() override;

private:
  static Napi::FunctionReference &constructor();

  static RTCSctpTransport *
  Create(PeerConnectionFactory *,
         rtc::scoped_refptr<webrtc::SctpTransportInterface>);

  Napi::Value GetTransport(const Napi::CallbackInfo &);
  Napi::Value GetState(const Napi::CallbackInfo &);
  Napi::Value GetMaxMessageSize(const Napi::CallbackInfo &);
  Napi::Value GetMaxChannels(const Napi::CallbackInfo &);

  rtc::scoped_refptr<webrtc::DtlsTransportInterface> _dtls_transport;
  RefPtr<PeerConnectionFactory> _factory;
  rtc::scoped_refptr<webrtc::SctpTransportInterface> _transport;

  OwnedWrap<RTCDtlsTransport> _transport_wrap;
};

} // namespace node_webrtc
