/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <iosfwd>
#include <memory>

#include <webrtc/api/data_channel_interface.h>
#include <webrtc/api/scoped_refptr.h>

#include "src/enums/node_webrtc/binary_type.hh"
#include "src/interfaces/rtc_peer_connection/peer_connection_factory.hh"
#include "src/node/async_object_wrap_with_loop.hh"
#include "src/node/event_queue.hh"
#include "src/node/ref_ptr.hh"
#include "src/node/wrap.hh"

namespace node_webrtc {

class DataChannelObserver;

class RTCDataChannel : public AsyncObjectWrapWithLoop<RTCDataChannel>,
                       public webrtc::DataChannelObserver {
  friend class node_webrtc::DataChannelObserver;

public:
  explicit RTCDataChannel(const Napi::CallbackInfo &);
  ~RTCDataChannel() override;

  RTCDataChannel(const RTCDataChannel &) = delete;
  RTCDataChannel(RTCDataChannel &&) = delete;
  RTCDataChannel &operator=(const RTCDataChannel &) = delete;
  RTCDataChannel &operator=(RTCDataChannel &&) = delete;

  static Napi::FunctionReference &constructor();

  static void Init(Napi::Env, Napi::Object);

  //
  // DataChannelObserver implementation.
  //
  void OnStateChange() override;
  void OnMessage(const webrtc::DataBuffer &buffer) override;

  void OnPeerConnectionClosed();

  static ::node_webrtc::Wrap<RTCDataChannel *,
                             rtc::scoped_refptr<webrtc::DataChannelInterface>,
                             node_webrtc::DataChannelObserver *> *
  wrap();

private:
  static RTCDataChannel *
  Create(node_webrtc::DataChannelObserver *,
         rtc::scoped_refptr<webrtc::DataChannelInterface>);

  static void HandleStateChange(RTCDataChannel &,
                                webrtc::DataChannelInterface::DataState);
  static void HandleMessage(RTCDataChannel &, const webrtc::DataBuffer &buffer);

  Napi::Value Send(const Napi::CallbackInfo &);
  Napi::Value Close(const Napi::CallbackInfo &);

  Napi::Value GetBufferedAmount(const Napi::CallbackInfo &);
  Napi::Value GetId(const Napi::CallbackInfo &);
  Napi::Value GetLabel(const Napi::CallbackInfo &);
  Napi::Value GetMaxPacketLifeTime(const Napi::CallbackInfo &);
  Napi::Value GetMaxRetransmits(const Napi::CallbackInfo &);
  Napi::Value GetNegotiated(const Napi::CallbackInfo &);
  Napi::Value GetOrdered(const Napi::CallbackInfo &);
  Napi::Value GetPriority(const Napi::CallbackInfo &);
  Napi::Value GetProtocol(const Napi::CallbackInfo &);
  Napi::Value GetBinaryType(const Napi::CallbackInfo &);
  Napi::Value GetReadyState(const Napi::CallbackInfo &);
  void SetBinaryType(const Napi::CallbackInfo &, const Napi::Value &);

  void CleanupInternals();

  BinaryType _binaryType;
  int _cached_id;
  std::string _cached_label;
  uint16_t _cached_max_packet_life_time;
  uint16_t _cached_max_retransmits;
  bool _cached_negotiated;
  bool _cached_ordered;
  std::string _cached_protocol;
  uint64_t _cached_buffered_amount;
  RefPtr<PeerConnectionFactory> _factory;
  rtc::scoped_refptr<webrtc::DataChannelInterface> _jingleDataChannel;
};

class DataChannelObserver : public EventQueue<RTCDataChannel>,
                            public webrtc::DataChannelObserver {
  friend class RTCDataChannel;

public:
  DataChannelObserver(
      PeerConnectionFactory *factory,
      rtc::scoped_refptr<webrtc::DataChannelInterface> jingleDataChannel);
  ~DataChannelObserver() override = default;

  DataChannelObserver(const DataChannelObserver &) = delete;
  DataChannelObserver(DataChannelObserver &&) = delete;
  DataChannelObserver &operator=(const DataChannelObserver &) = delete;
  DataChannelObserver &operator=(DataChannelObserver &&) = delete;

  void OnStateChange() override;
  void OnMessage(const webrtc::DataBuffer &buffer) override;

  rtc::scoped_refptr<webrtc::DataChannelInterface> channel() {
    return _jingleDataChannel;
  }

private:
  RefPtr<PeerConnectionFactory> _factory;
  rtc::scoped_refptr<webrtc::DataChannelInterface> _jingleDataChannel;
};

} // namespace node_webrtc
