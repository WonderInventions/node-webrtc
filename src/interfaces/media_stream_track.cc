/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/interfaces/media_stream_track.hh"

#include <node-addon-api/napi.h>
#include <webrtc/api/media_stream_interface.h>
#include <webrtc/api/peer_connection_interface.h>
#include <webrtc/rtc_base/helpers.h>

#include "src/converters.hh"
#include "src/converters/interfaces.hh"
#include "src/dictionaries/node_webrtc/media_track_settings.hh"
#include "src/interfaces/rtc_peer_connection/peer_connection_factory.hh"

namespace node_webrtc {

Napi::FunctionReference &MediaStreamTrack::constructor() {
  static Napi::FunctionReference constructor;
  return constructor;
}

MediaStreamTrack::MediaStreamTrack(const Napi::CallbackInfo &info)
    : AsyncObjectWrapWithLoop<MediaStreamTrack>("MediaStreamTrack", *this,
                                                info) {
  auto env = info.Env();

  if (info.Length() != 2 || !info[0].IsObject() || !info[1].IsExternal()) {
    Napi::TypeError::New(env, "You cannot construct a MediaStreamTrack")
        .ThrowAsJavaScriptException();
    return;
  }

  // FIXME(mroberts): There is a safer conversion here.
  _factory = PeerConnectionFactory::Unwrap(info[0].ToObject());
  auto track =
      *info[1]
           .As<Napi::External<
               rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>>()
           .Data();

  _track = std::move(track);
  _track->RegisterObserver(this);

  // NOTE(mroberts): This doesn't actually matter yet.
  _enabled = false;
}

MediaStreamTrack::~MediaStreamTrack() {
  _track = nullptr;

  Napi::HandleScope scope(PeerConnectionFactory::constructor().Env());

  wrap()->Release(this);
}

void MediaStreamTrack::Stop() {
  // SAFETY: `[=]` is a capture-by-copy, which will create a new reference to
  // the _track shared_ptr, so using that _track will be ok even if the current
  // object is destructed.
  // `this`'s pointer value needs to get copied for the `UnregisterObserver` to
  // work properly, but we actually don't care about use-after-free, since we
  // know when this is called the state should not be updating any more, and
  // we only need to remove it from the track's list.
  // This whole thing needs to be done because in M94, the `OnChanged()` method
  // becomes called from the worker thread, but UnregisterObserver wants to be
  // called on the signalling thread, and if they both do that at the same time,
  // there is a deadlock :)
  auto thisCopy = static_cast<webrtc::ObserverInterface *>(this);
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> trackCopy = _track;
  _factory->SignalingThread()->PostTask(RTC_FROM_HERE, [=]() { // NOLINT
    trackCopy->UnregisterObserver(thisCopy);
  });
  _ended = true;
  _enabled = _track->enabled();
  AsyncObjectWrapWithLoop<MediaStreamTrack>::Stop();
}

void MediaStreamTrack::OnChanged() {
  if (_track->state() ==
      webrtc::MediaStreamTrackInterface::TrackState::kEnded) {
    Stop();
  }
}

void MediaStreamTrack::OnPeerConnectionClosed() { Stop(); }

Napi::Value MediaStreamTrack::GetEnabled(const Napi::CallbackInfo &info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(
      info.Env(), _ended ? _enabled : _track->enabled(), result, Napi::Value)
  return result;
}

void MediaStreamTrack::SetEnabled(const Napi::CallbackInfo &info,
                                  const Napi::Value &value) {
  auto maybeEnabled = From<bool>(value);
  if (maybeEnabled.IsInvalid()) {
    Napi::TypeError::New(info.Env(), maybeEnabled.ToErrors()[0])
        .ThrowAsJavaScriptException();
    return;
  }
  auto enabled = maybeEnabled.UnsafeFromValid();
  if (_ended) {
    _enabled = enabled;
  } else {
    _track->set_enabled(enabled);
  }
}

Napi::Value MediaStreamTrack::GetId(const Napi::CallbackInfo &info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), _track->id(), result,
                                   Napi::Value)
  return result;
}

Napi::Value MediaStreamTrack::GetKind(const Napi::CallbackInfo &info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), _track->kind(), result,
                                   Napi::Value)
  return result;
}

Napi::Value MediaStreamTrack::GetReadyState(const Napi::CallbackInfo &info) {
  auto state = _ended ? webrtc::MediaStreamTrackInterface::TrackState::kEnded
                      : _track->state();
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), state, result, Napi::Value)
  return result;
}

Napi::Value MediaStreamTrack::GetMuted(const Napi::CallbackInfo &info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), false, result, Napi::Value)
  return result;
}

Napi::Value MediaStreamTrack::Clone(const Napi::CallbackInfo &) {
  auto label = rtc::CreateRandomUuid();
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> clonedTrack = nullptr;
  if (_track->kind() == _track->kAudioKind) {
    auto audioTrack = static_cast<webrtc::AudioTrackInterface *>(_track.get());
    clonedTrack =
        _factory->factory()->CreateAudioTrack(label, audioTrack->GetSource());
  } else {
    auto videoTrack = static_cast<webrtc::VideoTrackInterface *>(_track.get());
    clonedTrack =
        _factory->factory()->CreateVideoTrack(label, videoTrack->GetSource());
  }
  auto clonedMediaStreamTrack = wrap()->GetOrCreate(_factory, clonedTrack);
  if (_ended) {
    clonedMediaStreamTrack->Stop();
  }
  return clonedMediaStreamTrack->Value();
}

Napi::Value MediaStreamTrack::JsStop(const Napi::CallbackInfo &info) {
  Stop();
  return info.Env().Undefined();
}

Napi::Value MediaStreamTrack::GetSettings(const Napi::CallbackInfo &info) {
  // FIXME(jack): find some way to get the real settings.
  if (_track->kind() == _track->kAudioKind) {
    auto settings = AudioMediaTrackSettings{
        .deviceId = "FakeAudioDevice",
        .groupId = "FakeDeviceGroup",
        .autoGainControl = false,
        .echoCancellation = false,
        .noiseSuppression = false,
        .channelCount = 1,
        .sampleRate = 48000,
        .sampleSize = 16,
        .latency = 0.0,
        .volume = 0.25,
    };
    CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), settings, result, Napi::Value)
    return result;
  }
  assert(_track->kind() == _track->kVideoKind);
  auto settings = VideoMediaTrackSettings{
      .deviceId = "FakeVideoDevice",
      .groupId = "FakeDeviceGroup",
      .height = 480,
      .width = 640,
      .aspectRatio = 640.0 / 480.0,
      .frameRate = 30.0,
      .facingMode = "user",
  };
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), settings, result, Napi::Value)
  return result;
}

Wrap<MediaStreamTrack *, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>,
     PeerConnectionFactory *> *
MediaStreamTrack::wrap() {
  static auto wrap = new node_webrtc::Wrap<
      MediaStreamTrack *, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>,
      PeerConnectionFactory *>(MediaStreamTrack::Create);
  return wrap;
}

MediaStreamTrack *MediaStreamTrack::Create(
    PeerConnectionFactory *factory,
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) {
  auto env = constructor().Env();
  Napi::HandleScope scope(env);

  auto mediaStreamTrack = constructor().New(
      {factory->Value(),
       Napi::External<rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>::
           New(env, &track)});

  return Unwrap(mediaStreamTrack);
}

void MediaStreamTrack::Init(Napi::Env env, Napi::Object exports) {
  auto func = DefineClass(
      env, "MediaStreamTrack",
      {
          InstanceAccessor("enabled", &MediaStreamTrack::GetEnabled,
                           &MediaStreamTrack::SetEnabled),
          InstanceAccessor("id", &MediaStreamTrack::GetId, nullptr),
          InstanceAccessor("kind", &MediaStreamTrack::GetKind, nullptr),
          InstanceAccessor("readyState", &MediaStreamTrack::GetReadyState,
                           nullptr),
          InstanceAccessor("muted", &MediaStreamTrack::GetMuted, nullptr),
          InstanceMethod("clone", &MediaStreamTrack::Clone),
          InstanceMethod("stop", &MediaStreamTrack::JsStop),
          InstanceMethod("getSettings", &MediaStreamTrack::GetSettings),
      });

  constructor() = Napi::Persistent(func);
  constructor().SuppressDestruct();

  exports.Set("MediaStreamTrack", func);
}

CONVERTER_IMPL(MediaStreamTrack *,
               rtc::scoped_refptr<webrtc::AudioTrackInterface>,
               mediaStreamTrack) {
  auto track = mediaStreamTrack->track();
  if (track->kind() != webrtc::MediaStreamTrackInterface::kAudioKind) {
    return Validation<rtc::scoped_refptr<webrtc::AudioTrackInterface>>::Invalid(
        "Expected an audio MediaStreamTrack");
  }
  rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack(
      static_cast<webrtc::AudioTrackInterface *>(track.get()));
  return Pure(audioTrack);
}

CONVERTER_IMPL(MediaStreamTrack *,
               rtc::scoped_refptr<webrtc::VideoTrackInterface>,
               mediaStreamTrack) {
  auto track = mediaStreamTrack->track();
  if (track->kind() != webrtc::MediaStreamTrackInterface::kVideoKind) {
    return Validation<rtc::scoped_refptr<webrtc::VideoTrackInterface>>::Invalid(
        "Expected a video MediaStreamTrack");
  }
  rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack(
      static_cast<webrtc::VideoTrackInterface *>(track.get()));
  return Pure(videoTrack);
}

CONVERT_INTERFACE_TO_AND_FROM_NAPI(MediaStreamTrack, "MediaStreamTrack")

CONVERT_VIA(Napi::Value, MediaStreamTrack *,
            rtc::scoped_refptr<webrtc::AudioTrackInterface>)
CONVERT_VIA(Napi::Value, MediaStreamTrack *,
            rtc::scoped_refptr<webrtc::VideoTrackInterface>)

} // namespace node_webrtc
