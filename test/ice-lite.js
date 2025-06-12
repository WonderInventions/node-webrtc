"use strict";

const { RTCPeerConnection } = require("..");
const test = require("tape");

test("full peer connects to ICE-lite listener", async (t) => {
  t.plan(1);

  const lite = new RTCPeerConnection({ iceLite: true });
  const full = new RTCPeerConnection();

  lite.createDataChannel("dummy");

  full.onicecandidate = ({ candidate }) => {
    lite.addIceCandidate(candidate).catch(() => {});
  };

  await lite.setLocalDescription(await lite.createOffer());
  await full.setRemoteDescription(lite.localDescription);
  await full.setLocalDescription(await full.createAnswer());
  await lite.setRemoteDescription(full.localDescription);

  await new Promise((resolve, reject) => {
    full.oniceconnectionstatechange = () => {
      if (
        full.iceConnectionState === "connected" ||
        full.iceConnectionState === "completed"
      )
        resolve();
      if (full.iceConnectionState === "failed") reject(new Error("ICE failed"));
    };
  });

  t.pass('dialer reached "connected" with ICE-lite listener');

  lite.close();
  full.close();
});

test("ICE-lite listener works as answerer", async (t) => {
  t.plan(1);

  const lite = new RTCPeerConnection({ iceLite: true });
  const full = new RTCPeerConnection();

  full.createDataChannel("x");

  full.onicecandidate = ({ candidate }) =>
    lite.addIceCandidate(candidate).catch(() => {});

  await full.setLocalDescription(await full.createOffer());
  await lite.setRemoteDescription(full.localDescription);

  await lite.setLocalDescription(await lite.createAnswer());
  await full.setRemoteDescription(lite.localDescription);

  await new Promise((resolve, reject) => {
    full.oniceconnectionstatechange = () => {
      const s = full.iceConnectionState;
      if (s === "connected" || s === "completed") resolve();
      if (s === "failed") reject(new Error("ICE failed"));
    };
  });

  t.pass("connected with ICE-lite answerer");
  lite.close();
  full.close();
});
