/* globals gc */
"use strict";

const test = require("tape");

const RTCVideoSource = require("..").nonstandard.RTCVideoSource;

const {
  confirmSentFrameDimensions,
  negotiateRTCPeerConnections,
} = require("./lib/pc");

const { I420Frame } = require("./lib/frame");

const frame = new I420Frame(640, 480);

function tick() {
  return new Promise((resolve) => setTimeout(resolve));
}

function printSource(source) {
  console.log(source);
}

function printTrack(track) {
  console.log(track);
}

test("simple usage", async (t) => {
  await (async () => {
    const source = new RTCVideoSource();
    printSource(source);
    await tick();

    source.onFrame(frame);
    await tick();

    const track = source.createTrack();
    printTrack(track);
    await tick();

    const clonedTrack = track.clone();
    printTrack(clonedTrack);
    await tick();

    source.onFrame(frame);
    await tick();

    track.stop();
    printTrack(track);
    await tick();

    source.onFrame(frame);
    await tick();

    clonedTrack.stop();
    printTrack(clonedTrack);
    await tick();

    source.onFrame(frame);
    await tick();
  })();

  if (typeof gc === "function") {
    gc();
  }

  t.end();
});

test("getStats()", async (t) => {
  const source = new RTCVideoSource();
  const track = source.createTrack();

  const [pc1, pc2] = await negotiateRTCPeerConnections({
    withPc1(pc1) {
      pc1.addTrack(track);
    },
  });

  const frames = [
    new I420Frame(640, 480),
    new I420Frame(1280, 720),
    new I420Frame(320, 240),
  ];

  for (const frame of frames) {
    await confirmSentFrameDimensions(source, track, pc1, frame);
    t.pass(`Sent a ${frame.width}x${frame.height} frame`);
  }

  track.stop();
  pc1.close();
  pc2.close();

  t.end();
});

test("constructor", (t) => {
  const source1 = new RTCVideoSource();
  t.equal(source1.needsDenoising, null);
  t.equal(source1.isScreencast, false);

  const source2 = new RTCVideoSource({ needsDenoising: true });
  t.equal(source2.needsDenoising, true);
  t.equal(source2.isScreencast, false);

  const source3 = new RTCVideoSource({ needsDenoising: false });
  t.equal(source3.needsDenoising, false);
  t.equal(source3.isScreencast, false);

  const source4 = new RTCVideoSource({ isScreencast: true });
  t.equal(source4.needsDenoising, null);
  t.equal(source4.isScreencast, true);

  t.end();
});
