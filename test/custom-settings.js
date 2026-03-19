"use strict";

const tape = require("tape");
const wrtc = require("..");

const RTCPeerConnection = wrtc.RTCPeerConnection;

async function connectWithConfig(config) {
  let resolve;
  let reject;
  const done = new Promise((res, rej) => {
    resolve = res;
    reject = rej;
  });

  const pc1 = new RTCPeerConnection({ iceServers: [] });
  const pc2 = new RTCPeerConnection(Object.assign({ iceServers: [] }, config));

  pc1.onicecandidate = function (e) {
    if (e.candidate) pc2.addIceCandidate(e.candidate);
  };

  pc2.onicecandidate = function (e) {
    if (e.candidate) {
      if (config.portRange) {
        const { min, max } = config.portRange;
        const port = parsePort(e.candidate.candidate);
        if (port < min || port > max) {
          pc1.close();
          pc2.close();
          reject(
            new Error(
              `candidate port ${port} outside range ${min} - ${max}: ${e.candidate.candidate}`,
            ),
          );
          return;
        }
      }
      pc1.addIceCandidate(e.candidate);
    }
  };

  const dc = pc1.createDataChannel("test");
  pc2.ondatachannel = function (evt) {
    evt.channel.onmessage = function () {
      pc1.close();
      pc2.close();
      resolve();
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

function parsePort(candidate) {
  const match = candidate.match(
    /candidate:\S+\s\d+\s\S+\s\d+\s\S+\s(\d+)\styp/,
  );
  return match ? parseInt(match[1], 10) : -1;
}

tape("custom ports connect once", async function (t) {
  t.plan(1);
  try {
    await connectWithConfig({ portRange: { min: 9000, max: 9010 } });

    t.pass("ConnectClientServer pass");
  } catch (err) {
    t.error(err, "connectClientServer callback");
  }
});

tape("custom ports connect concurrently", async function (t) {
  const n = 2;
  t.plan(n);
  let promises = [];

  for (let i = 0; i < n; i++) {
    promises.push(
      connectWithConfig({ portRange: { min: 9000, max: 9010 } })
        .then(function () {
          t.pass("connectClientServer pass");
        })
        .catch(function (err) {
          t.error(err, "connectClientServer error");
        }),
    );
  }

  await Promise.all(promises);
});
