/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/methods/get_user_media.hh"

#include <webrtc/api/audio_options.h>
#include <webrtc/api/peer_connection_interface.h>

#include "src/converters.hh"
#include "src/converters/arguments.hh"
#include "src/converters/object.hh"
#include "src/functional/curry.hh"
#include "src/functional/either.hh"
#include "src/functional/maybe.hh"
#include "src/functional/operators.hh"
#include "src/functional/validation.hh"
#include "src/interfaces/media_stream.hh"
#include "src/interfaces/rtc_peer_connection/peer_connection_factory.hh"
#include "src/interfaces/rtc_video_source.hh"
#include "src/node/utility.hh"

// TODO(mroberts): Expand support for other members.
struct MediaTrackConstraintSet {
  node_webrtc::Maybe<uint16_t> width;
  node_webrtc::Maybe<uint16_t> height;

  static MediaTrackConstraintSet
  Create(const node_webrtc::Maybe<uint16_t> width,
         const node_webrtc::Maybe<uint16_t> height) {
    return {width, height};
  }
};

template <>
struct node_webrtc::Converter<Napi::Value, MediaTrackConstraintSet> {
  static node_webrtc::Validation<MediaTrackConstraintSet>
  Convert(const Napi::Value value) {
    return node_webrtc::From<Napi::Object>(value)
        .FlatMap<MediaTrackConstraintSet>([](auto object) {
          return curry(MediaTrackConstraintSet::Create) %
                 node_webrtc::GetOptional<uint16_t>(object, "width") *
                 node_webrtc::GetOptional<uint16_t>(object, "height");
        });
  }
};

struct MediaTrackConstraints : public MediaTrackConstraintSet {
  std::vector<MediaTrackConstraintSet> advanced;

  static MediaTrackConstraints
  Create(const MediaTrackConstraintSet set,
         const std::vector<MediaTrackConstraintSet> &advanced) {
    MediaTrackConstraints constraints;
    constraints.width = set.width;
    constraints.height = set.height;
    constraints.advanced = advanced;
    return constraints;
  }
};

template <> struct node_webrtc::Converter<Napi::Value, MediaTrackConstraints> {
  static node_webrtc::Validation<MediaTrackConstraints>
  Convert(const Napi::Value value) {
    return node_webrtc::From<Napi::Object>(value)
        .FlatMap<MediaTrackConstraints>([&value](auto object) {
          return curry(MediaTrackConstraints::Create) %
                 node_webrtc::From<MediaTrackConstraintSet>(value) *
                 node_webrtc::GetOptional<std::vector<MediaTrackConstraintSet>>(
                     object, "advanced",
                     std::vector<MediaTrackConstraintSet>());
        });
  }
};

struct MediaStreamConstraints {
  node_webrtc::Maybe<node_webrtc::Either<bool, MediaTrackConstraints>> audio;
  node_webrtc::Maybe<node_webrtc::Either<bool, MediaTrackConstraints>> video;

  static node_webrtc::Validation<MediaStreamConstraints> Create(
      const node_webrtc::Maybe<node_webrtc::Either<bool, MediaTrackConstraints>>
          &audio,
      const node_webrtc::Maybe<node_webrtc::Either<bool, MediaTrackConstraints>>
          &video) {
    return audio.IsNothing() && video.IsNothing()
               ? node_webrtc::Validation<MediaStreamConstraints>::Invalid(
                     R"(Must specify at least "audio" or "video")")
               : node_webrtc::Validation<MediaStreamConstraints>::Valid(
                     {audio, video});
  }
};

template <> struct node_webrtc::Converter<Napi::Value, MediaStreamConstraints> {
  static node_webrtc::Validation<MediaStreamConstraints>
  Convert(const Napi::Value value) {
    return node_webrtc::From<Napi::Object>(value)
        .FlatMap<MediaStreamConstraints>([](auto object) {
          return node_webrtc::Validation<MediaStreamConstraints>::Join(
              curry(MediaStreamConstraints::Create) %
              node_webrtc::GetOptional<
                  node_webrtc::Either<bool, MediaTrackConstraints>>(object,
                                                                    "audio") *
              node_webrtc::GetOptional<
                  node_webrtc::Either<bool, MediaTrackConstraints>>(object,
                                                                    "video"));
        });
  }
};

Napi::Value
node_webrtc::GetUserMedia::GetUserMediaImpl(const Napi::CallbackInfo &info) {
  CREATE_DEFERRED(info.Env(), deferred)

  CONVERT_ARGS_OR_REJECT_AND_RETURN_NAPI(deferred, info, constraints,
                                         MediaStreamConstraints)

  auto factory = node_webrtc::PeerConnectionFactory::GetOrCreateDefault();
  auto stream =
      factory->factory()->CreateLocalMediaStream(rtc::CreateRandomUuid());

  auto audio =
      constraints.audio
          .Map([](auto constraint) { return constraint.FromLeft(true); })
          .FromMaybe(false);

  auto video =
      constraints.video
          .Map([](auto constraint) { return constraint.FromLeft(true); })
          .FromMaybe(false);

  if (audio) {
    cricket::AudioOptions options;
    auto source = factory->factory()->CreateAudioSource(options);
    auto track =
        factory->factory()->CreateAudioTrack(rtc::CreateRandomUuid(), source);
    stream->AddTrack(track);
  }

  if (video) {
    auto source = new rtc::RefCountedObject<node_webrtc::RTCVideoTrackSource>();
    auto track =
        factory->factory()->CreateVideoTrack(rtc::CreateRandomUuid(), source);
    stream->AddTrack(track);
  }

  auto media_stream =
      RefPtr<MediaStream>(MediaStream::wrap()->GetOrCreate(factory, stream));
  // TODO(jack): this may end up being the wrong lifetime, if Resolve can't
  // automatically Ref it...
  node_webrtc::Resolve(deferred, media_stream.ptr());
  return deferred.Promise();
}

void node_webrtc::GetUserMedia::Init(Napi::Env env, Napi::Object exports) {
  exports.Set("getUserMedia", Napi::Function::New(env, GetUserMediaImpl));
}
