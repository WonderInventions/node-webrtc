/* Copyright (c) 2026 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <memory>

#include <webrtc/rtc_base/ssl_certificate.h>

namespace node_webrtc {

std::unique_ptr<webrtc::SSLCertificateVerifier>
CreateWindowsPlatformCertificateVerifier();

}  // namespace node_webrtc
