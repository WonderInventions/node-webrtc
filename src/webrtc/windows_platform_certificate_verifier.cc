/* Copyright (c) 2026 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/webrtc/windows_platform_certificate_verifier.hh"

#if defined(WEBRTC_WIN)

#include <windows.h>
#include <wincrypt.h>

#include <memory>
#include <string>
#include <vector>

#include <webrtc/rtc_base/buffer.h>
#include <webrtc/rtc_base/logging.h>

namespace node_webrtc {
namespace {

struct CertContextDeleter {
  void operator()(const CERT_CONTEXT* cert) const {
    if (cert != nullptr) {
      CertFreeCertificateContext(cert);
    }
  }
};

struct CertStoreDeleter {
  void operator()(const void* store) const {
    if (store != nullptr) {
      CertCloseStore(static_cast<HCERTSTORE>(const_cast<void*>(store)), 0);
    }
  }
};

struct CertChainContextDeleter {
  void operator()(const CERT_CHAIN_CONTEXT* chain) const {
    if (chain != nullptr) {
      CertFreeCertificateChain(chain);
    }
  }
};

using UniqueCertContext =
    std::unique_ptr<const CERT_CONTEXT, CertContextDeleter>;
using UniqueCertStore = std::unique_ptr<const void, CertStoreDeleter>;
using UniqueCertChainContext =
    std::unique_ptr<const CERT_CHAIN_CONTEXT, CertChainContextDeleter>;

UniqueCertContext CreateCertificateContext(
    const webrtc::SSLCertificate& certificate) {
  webrtc::Buffer der;
  certificate.ToDER(&der);
  if (der.empty()) {
    RTC_LOG(LS_WARNING)
        << "WindowsPlatformCertificateVerifier: empty DER certificate";
    return UniqueCertContext(nullptr);
  }

  return UniqueCertContext(CertCreateCertificateContext(
      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
      reinterpret_cast<const BYTE*>(der.data()),
      static_cast<DWORD>(der.size())));
}

class WindowsPlatformCertificateVerifier final
    : public webrtc::SSLCertificateVerifier {
 public:
  bool VerifyChain(const webrtc::SSLCertChain& chain) override {
    if (chain.GetSize() == 0) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: empty certificate chain";
      return false;
    }

    UniqueCertContext leaf_context(CreateCertificateContext(chain.Get(0)));
    if (!leaf_context) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: failed to create leaf "
             "certificate context, error="
          << GetLastError();
      return false;
    }

    UniqueCertStore intermediate_store(
        CertOpenStore(CERT_STORE_PROV_MEMORY, 0, 0,
                      CERT_STORE_CREATE_NEW_FLAG, nullptr));
    if (!intermediate_store) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: failed to create "
             "intermediate store, error="
          << GetLastError();
      return false;
    }

    for (size_t i = 1; i < chain.GetSize(); ++i) {
      UniqueCertContext cert_context(CreateCertificateContext(chain.Get(i)));
      if (!cert_context) {
        RTC_LOG(LS_WARNING)
            << "WindowsPlatformCertificateVerifier: failed to create "
               "intermediate certificate context at index "
            << i << ", error=" << GetLastError();
        return false;
      }

      if (!CertAddCertificateContextToStore(
              static_cast<HCERTSTORE>(const_cast<void*>(intermediate_store.get())),
              cert_context.get(), CERT_STORE_ADD_ALWAYS, nullptr)) {
        RTC_LOG(LS_WARNING)
            << "WindowsPlatformCertificateVerifier: failed to add "
               "intermediate certificate to store at index "
            << i << ", error=" << GetLastError();
        return false;
      }
    }

    LPCSTR usage_identifiers[] = {szOID_PKIX_KP_SERVER_AUTH};
    CERT_CHAIN_PARA chain_parameters = {};
    chain_parameters.cbSize = sizeof(chain_parameters);
    chain_parameters.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
    chain_parameters.RequestedUsage.Usage.cUsageIdentifier = 1;
    chain_parameters.RequestedUsage.Usage.rgpszUsageIdentifier =
        const_cast<LPSTR*>(usage_identifiers);

    PCCERT_CHAIN_CONTEXT raw_chain_context = nullptr;
    if (!CertGetCertificateChain(
            nullptr, leaf_context.get(), nullptr,
            static_cast<HCERTSTORE>(const_cast<void*>(intermediate_store.get())),
            &chain_parameters, 0, nullptr, &raw_chain_context)) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: CertGetCertificateChain "
             "failed, error="
          << GetLastError();
      return false;
    }

    UniqueCertChainContext chain_context(raw_chain_context);

    CERT_CHAIN_POLICY_PARA policy_parameters = {};
    policy_parameters.cbSize = sizeof(policy_parameters);

    CERT_CHAIN_POLICY_STATUS policy_status = {};
    policy_status.cbSize = sizeof(policy_status);

    if (!CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_BASE,
                                          chain_context.get(),
                                          &policy_parameters,
                                          &policy_status)) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: "
             "CertVerifyCertificateChainPolicy failed, error="
          << GetLastError();
      return false;
    }

    if (policy_status.dwError != 0) {
      RTC_LOG(LS_WARNING)
          << "WindowsPlatformCertificateVerifier: certificate chain rejected, "
             "policy_error="
          << policy_status.dwError;
      return false;
    }

    RTC_LOG(LS_INFO)
        << "WindowsPlatformCertificateVerifier: platform trust validation ok";
    return true;
  }
};

}  // namespace

std::unique_ptr<webrtc::SSLCertificateVerifier>
CreateWindowsPlatformCertificateVerifier() {
  return std::make_unique<WindowsPlatformCertificateVerifier>();
}

}  // namespace node_webrtc

#else

namespace node_webrtc {

std::unique_ptr<webrtc::SSLCertificateVerifier>
CreateWindowsPlatformCertificateVerifier() {
  return nullptr;
}

}  // namespace node_webrtc

#endif
