"use strict";

const tape = require("tape");
const wrtc = require("..");

const RTCPeerConnection = wrtc.RTCPeerConnection;

async function connect() {
  let resolve;
  let reject;
  const done = new Promise((res, rej) => {
    resolve = res;
    reject = rej;
  });
  const pc1 = new RTCPeerConnection({ iceServers: [] });
  const pc2 = new RTCPeerConnection({ iceServers: [] });

  pc1.onicecandidate = function (e) {
    if (e.candidate) pc2.addIceCandidate(e.candidate);
  };
  pc2.onicecandidate = function (e) {
    if (e.candidate) pc1.addIceCandidate(e.candidate);
  };

  const dc = pc1.createDataChannel("test");

  pc2.ondatachannel = function (evt) {
    evt.channel.onmessage = function (msg) {
      pc1.close();
      pc2.close();
      resolve(msg.data);
    };
  };

  dc.onopen = function () {
    dc.send("hello");
  };

  try {
    const offer = await pc1.createOffer();
    await pc1.setLocalDescription(offer);
    await pc2.setRemoteDescription(pc1.localDescription);
    const answer = await pc2.createAnswer();
    await pc2.setLocalDescription(answer);
    await pc1.setRemoteDescription(pc2.localDescription);
  } catch (err) {
    pc1.close();
    pc2.close();
    reject(err);
  }

  return done;
}

async function connectLoop(count) {
  for (let i = 0; i < count; i++) {
    await connect();
  }
}

tape("connect once", async function (t) {
  t.plan(1);
  try {
    await connect();
    t.pass("connect once pass");
  } catch (err) {
    t.error(err, "connect once callback");
  }
});

tape("connect loop", async function (t) {
  t.plan(1);
  try {
    await connectLoop(10);
    t.pass("connect loop completed");
  } catch (err) {
    t.error(err, "connect loop callback");
  }
});

tape("connect concurrent", async function (t) {
  const n = 10;
  t.plan(n);

  let promises = [];
  for (let i = 0; i < n; i++) {
    promises.push(
      connect()
        .then(function () {
          t.pass("connect concurrent pass");
        })
        .catch(function (err) {
          t.error(err, "connect concurrent error");
        }),
    );
  }

  await Promise.all(promises);
});

tape("connect loop concurrent", async function (t) {
  const n = 10;
  t.plan(n);

  let promises = [];
  for (let i = 0; i < n; i++) {
    promises.push(
      connectLoop(n)
        .then(function () {
          t.pass("connectLoop concurrent pass");
        })
        .catch(function (err) {
          t.error(err, "connectLoop concurrent error");
        }),
    );
  }

  await Promise.all(promises);
});
