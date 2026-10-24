/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <node-addon-api/napi.h>
#include <webrtc/api/scoped_refptr.h>
#include <webrtc/api/stats/rtc_stats_collector_callback.h>

#include "src/interfaces/rtc_peer_connection.hh" // IWYU pragma: keep
#include "src/node/promise.hh"

namespace webrtc {
class RTCStatsReport;
}

namespace node_webrtc {

class RTCStatsCollector : public PromiseCreator<RTCPeerConnection>,
                          public webrtc::RTCStatsCollectorCallback {
public:
  // libwebrtc's Close() delivers pending stats after the closed signaling
  // state, which stops the peer connection's EventLoop. Dispatch the queued
  // result anyway so that getStats() resolves.
  RTCStatsCollector(RTCPeerConnection *peer_connection,
                    Napi::Promise::Deferred deferred)
      : PromiseCreator<RTCPeerConnection>(peer_connection, deferred, true) {}

  void OnStatsDelivered(
      const rtc::scoped_refptr<const webrtc::RTCStatsReport> &) override;
};

} // namespace node_webrtc
