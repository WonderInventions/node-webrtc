/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <vector>

#include <node-addon-api/napi.h>
#include <webrtc/api/peer_connection_interface.h>
#include <webrtc/api/scoped_refptr.h>

#include "src/dictionaries/node_webrtc/extended_rtc_configuration.hh"
#include "src/dictionaries/node_webrtc/rtc_session_description_init.hh"
#include "src/interfaces/media_stream.hh"
#include "src/interfaces/rtc_data_channel.hh"
#include "src/interfaces/rtc_rtp_receiver.hh"
#include "src/interfaces/rtc_rtp_sender.hh"
#include "src/interfaces/rtc_rtp_transceiver.hh"
#include "src/interfaces/rtc_sctp_transport.hh"
#include "src/node/async_object_wrap_with_loop.hh"
#include "src/node/ref_ptr.hh"
#include "src/node/wrap.hh"

namespace webrtc {

class DataChannelInterface;
class IceCandidateInterface;
class MediaStreamInterface;
class RtpReceiverInterface;
class RtpTransceiverInterface;

} // namespace webrtc

namespace node_webrtc {

class PeerConnectionFactory;

class RTCPeerConnection : public AsyncObjectWrapWithLoop<RTCPeerConnection>,
                          public webrtc::PeerConnectionObserver {
public:
  RTCPeerConnection(const RTCPeerConnection &) = delete;
  RTCPeerConnection(RTCPeerConnection &&) = delete;
  RTCPeerConnection &operator=(const RTCPeerConnection &) = delete;
  RTCPeerConnection &operator=(RTCPeerConnection &&) = delete;
  explicit RTCPeerConnection(const Napi::CallbackInfo &);

  ~RTCPeerConnection() override;

  //
  // PeerConnectionObserver implementation.
  //
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override;
  void OnIceCandidateError(const std::string &address, int port,
                           const std::string &url, int error_code,
                           const std::string &error_text) override;
  void OnRenegotiationNeeded() override;

  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;

  void
  OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnRemoveStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;

  void
  OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
             const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>
                 &streams) override;
  void OnTrack(
      rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;

  static void Init(Napi::Env, Napi::Object);

  static Napi::FunctionReference &constructor();

  void SaveLastSdp(const RTCSessionDescriptionInit &lastSdp);

private:
  Napi::Value AddTrack(const Napi::CallbackInfo &);
  Napi::Value AddTransceiver(const Napi::CallbackInfo &);
  Napi::Value RemoveTrack(const Napi::CallbackInfo &);
  Napi::Value CreateOffer(const Napi::CallbackInfo &);
  Napi::Value CreateAnswer(const Napi::CallbackInfo &);
  Napi::Value SetLocalDescription(const Napi::CallbackInfo &);
  Napi::Value SetRemoteDescription(const Napi::CallbackInfo &);
  Napi::Value UpdateIce(const Napi::CallbackInfo &);
  Napi::Value AddIceCandidate(const Napi::CallbackInfo &);
  Napi::Value CreateDataChannel(const Napi::CallbackInfo &);
  /*
  Napi::Value GetLocalStreams(const Napi::CallbackInfo&);
  Napi::Value GetRemoteStreams(const Napi::CallbackInfo&);
  Napi::Value GetStreamById(const Napi::CallbackInfo&);
  Napi::Value AddStream(const Napi::CallbackInfo&);
  Napi::Value RemoveStream(const Napi::CallbackInfo&);
  */
  Napi::Value GetConfiguration(const Napi::CallbackInfo &);
  Napi::Value SetConfiguration(const Napi::CallbackInfo &);
  Napi::Value GetReceivers(const Napi::CallbackInfo &);
  Napi::Value GetSenders(const Napi::CallbackInfo &);
  Napi::Value GetStats(const Napi::CallbackInfo &);
  Napi::Value GetTransceivers(const Napi::CallbackInfo &);
  Napi::Value Close(const Napi::CallbackInfo &);
  Napi::Value RestartIce(const Napi::CallbackInfo &);

  Napi::Value GetCanTrickleIceCandidates(const Napi::CallbackInfo &);
  Napi::Value GetConnectionState(const Napi::CallbackInfo &);
  Napi::Value GetCurrentLocalDescription(const Napi::CallbackInfo &);
  Napi::Value GetLocalDescription(const Napi::CallbackInfo &);
  Napi::Value GetPendingLocalDescription(const Napi::CallbackInfo &);
  Napi::Value GetCurrentRemoteDescription(const Napi::CallbackInfo &);
  Napi::Value GetRemoteDescription(const Napi::CallbackInfo &);
  Napi::Value GetPendingRemoteDescription(const Napi::CallbackInfo &);
  Napi::Value GetIceConnectionState(const Napi::CallbackInfo &);
  Napi::Value GetSctp(const Napi::CallbackInfo &);
  Napi::Value GetSignalingState(const Napi::CallbackInfo &);
  Napi::Value GetIceGatheringState(const Napi::CallbackInfo &);

  RTCSessionDescriptionInit _lastSdp;

  UnsignedShortRange _port_range;
  ExtendedRTCConfiguration _cached_configuration;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> _jinglePeerConnection;

  // TODO(jack): make this a RefPtr if we ever stop using the default global
  // PeerConnectionFactory
  PeerConnectionFactory *_factory;
  bool _shouldReleaseFactory;

  std::vector<RTCDataChannel *> _channels;
  OwnedWrap<RTCDataChannel> _data_channel_wrap;
  OwnedWrap<MediaStream> _stream_wrap;
  OwnedWrap<RTCRtpReceiver> _receiver_wrap;
  OwnedWrap<RTCRtpTransceiver> _transceiver_wrap;
  OwnedWrap<RTCRtpSender> _sender_wrap;
  OwnedWrap<RTCSctpTransport> _transport_wrap;
};

} // namespace node_webrtc
