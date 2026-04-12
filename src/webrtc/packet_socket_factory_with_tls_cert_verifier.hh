/* Copyright (c) 2026 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <memory>

#include <webrtc/api/packet_socket_factory.h>

namespace node_webrtc {

class PacketSocketFactoryWithTlsCertVerifier final
    : public webrtc::PacketSocketFactory {
 public:
  explicit PacketSocketFactoryWithTlsCertVerifier(
      std::unique_ptr<webrtc::PacketSocketFactory> inner);
  ~PacketSocketFactoryWithTlsCertVerifier() override;

  std::unique_ptr<webrtc::AsyncPacketSocket> CreateUdpSocket(
      const webrtc::Environment& env,
      const webrtc::SocketAddress& address,
      uint16_t min_port,
      uint16_t max_port) override;
  std::unique_ptr<webrtc::AsyncListenSocket> CreateServerTcpSocket(
      const webrtc::Environment& env,
      const webrtc::SocketAddress& local_address,
      uint16_t min_port,
      uint16_t max_port,
      int opts) override;
  std::unique_ptr<webrtc::AsyncPacketSocket> CreateClientTcpSocket(
      const webrtc::Environment& env,
      const webrtc::SocketAddress& local_address,
      const webrtc::SocketAddress& remote_address,
      const webrtc::PacketSocketTcpOptions& tcp_options) override;
  std::unique_ptr<webrtc::AsyncDnsResolverInterface> CreateAsyncDnsResolver()
      override;
  std::unique_ptr<webrtc::AsyncPacketSocket> CreateClientUdpSocket(
      const webrtc::Environment& env,
      const webrtc::SocketAddress& local_address,
      const webrtc::SocketAddress& remote_address,
      uint16_t min_port,
      uint16_t max_port,
      const webrtc::PacketSocketTcpOptions& options) override;

 private:
  webrtc::PacketSocketTcpOptions WithTlsVerifier(
      const webrtc::PacketSocketTcpOptions& options) const;

  std::unique_ptr<webrtc::PacketSocketFactory> inner_;
  std::unique_ptr<webrtc::SSLCertificateVerifier> tls_cert_verifier_;
};

}  // namespace node_webrtc
