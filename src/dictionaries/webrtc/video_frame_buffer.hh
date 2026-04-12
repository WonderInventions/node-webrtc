#pragma once

#include "src/converters.hh"
#include "src/converters/napi.hh"

namespace webrtc {
template <typename T> class scoped_refptr;
class I420Buffer;
}
namespace webrtc {
class I420BufferInterface;
}
namespace webrtc {
class VideoFrameBuffer;
}

namespace node_webrtc {

class I420ImageData;

DECLARE_CONVERTER(I420ImageData, webrtc::scoped_refptr<webrtc::I420Buffer>)

DECLARE_FROM_NAPI(webrtc::scoped_refptr<webrtc::I420Buffer>)
DECLARE_TO_NAPI(const webrtc::I420BufferInterface *)
DECLARE_TO_NAPI(webrtc::scoped_refptr<webrtc::VideoFrameBuffer>)

} // namespace node_webrtc
