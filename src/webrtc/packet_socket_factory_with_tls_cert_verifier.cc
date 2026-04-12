/* Copyright (c) 2026 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/webrtc/packet_socket_factory_with_tls_cert_verifier.hh"

#include <utility>

#include <webrtc/rtc_base/logging.h>

#include "src/webrtc/windows_platform_certificate_verifier.hh"

namespace node_webrtc {

PacketSocketFactoryWithTlsCertVerifier::PacketSocketFactoryWithTlsCertVerifier(
    std::unique_ptr<webrtc::PacketSocketFactory> inner)
    : inner_(std::move(inner)),
      tls_cert_verifier_(CreateWindowsPlatformCertificateVerifier()) {}

PacketSocketFactoryWithTlsCertVerifier::~PacketSocketFactoryWithTlsCertVerifier() =
    default;

std::unique_ptr<webrtc::AsyncPacketSocket>
PacketSocketFactoryWithTlsCertVerifier::CreateUdpSocket(
    const webrtc::Environment& env,
    const webrtc::SocketAddress& address,
    uint16_t min_port,
    uint16_t max_port) {
  return inner_->CreateUdpSocket(env, address, min_port, max_port);
}

std::unique_ptr<webrtc::AsyncListenSocket>
PacketSocketFactoryWithTlsCertVerifier::CreateServerTcpSocket(
    const webrtc::Environment& env,
    const webrtc::SocketAddress& local_address,
    uint16_t min_port,
    uint16_t max_port,
    int opts) {
  return inner_->CreateServerTcpSocket(env, local_address, min_port, max_port,
                                       opts);
}

std::unique_ptr<webrtc::AsyncPacketSocket>
PacketSocketFactoryWithTlsCertVerifier::CreateClientTcpSocket(
    const webrtc::Environment& env,
    const webrtc::SocketAddress& local_address,
    const webrtc::SocketAddress& remote_address,
    const webrtc::PacketSocketTcpOptions& tcp_options) {
  return inner_->CreateClientTcpSocket(env, local_address, remote_address,
                                       WithTlsVerifier(tcp_options));
}

std::unique_ptr<webrtc::AsyncDnsResolverInterface>
PacketSocketFactoryWithTlsCertVerifier::CreateAsyncDnsResolver() {
  return inner_->CreateAsyncDnsResolver();
}

std::unique_ptr<webrtc::AsyncPacketSocket>
PacketSocketFactoryWithTlsCertVerifier::CreateClientUdpSocket(
    const webrtc::Environment& env,
    const webrtc::SocketAddress& local_address,
    const webrtc::SocketAddress& remote_address,
    uint16_t min_port,
    uint16_t max_port,
    const webrtc::PacketSocketTcpOptions& options) {
  return inner_->CreateClientUdpSocket(env, local_address, remote_address,
                                       min_port, max_port,
                                       WithTlsVerifier(options));
}

webrtc::PacketSocketTcpOptions
PacketSocketFactoryWithTlsCertVerifier::WithTlsVerifier(
    const webrtc::PacketSocketTcpOptions& options) const {
  auto wrapped = options;

#if defined(WEBRTC_WIN)
  const bool wants_secure_tls =
      (options.opts & webrtc::PacketSocketFactory::OPT_TLS) != 0;
  if (wants_secure_tls && wrapped.tls_cert_verifier == nullptr &&
      tls_cert_verifier_ != nullptr) {
    RTC_LOG(LS_INFO)
        << "PacketSocketFactoryWithTlsCertVerifier: injecting Windows TLS "
           "certificate verifier";
    wrapped.tls_cert_verifier = tls_cert_verifier_.get();
  }
#endif

  return wrapped;
}

}  // namespace node_webrtc
