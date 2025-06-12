"use strict";

const test = require("tape");
const { RTCPeerConnection } = require("..");

function mungeFingerprint(sdp) {
  return sdp.replace(
    /^a=fingerprint:sha-256 [0-9A-F:]+/im,
    "a=fingerprint:sha-256 " + Array(63).fill("AB").join(":"),
  );
}

/**
 * Connect two peers, optionally SDP-munge *receiver* side.
 * `cfgB` is where we can pass disableFingerprintVerification.
 */
async function connectPeers({ munge = false, cfgB = {} } = {}) {
  const A = new RTCPeerConnection();
  const B = new RTCPeerConnection(cfgB);

  A.createDataChannel("d");

  // pipe candidates both ways
  A.onicecandidate = ({ candidate }) =>
    B.addIceCandidate(candidate || null).catch(() => {});
  B.onicecandidate = ({ candidate }) =>
    A.addIceCandidate(candidate || null).catch(() => {});

  await A.setLocalDescription(await A.createOffer());
  let sdp = A.localDescription.sdp;
  if (munge) sdp = mungeFingerprint(sdp);

  await B.setRemoteDescription({ type: "offer", sdp });
  await B.setLocalDescription(await B.createAnswer());
  await A.setRemoteDescription(B.localDescription);

  return new Promise((resolve, reject) => {
    const timeout = setTimeout(() => reject(new Error("timeout")), 8000);
    function done(_, ok) {
      clearTimeout(timeout);
      if (ok) {
        resolve();
      } else {
        reject(new Error("failed"));
      }
    }
    A.onconnectionstatechange = () => {
      if (A.connectionState === "connected") done(A, true);
      if (A.connectionState === "failed") done(A, false);
    };
  }).finally(() => {
    A.close();
    B.close();
  });
}

test("fingerprint mismatch - default config should fail", async (t) => {
  t.plan(1);
  try {
    await connectPeers({ munge: true });
    t.fail("unexpectedly succeeded");
  } catch (e) {
    console.log(e);
    t.pass("connection failed as expected");
  }
});

test("fingerprint mismatch - disableFingerprintVerification:true should succeed", async (t) => {
  t.plan(1);
  try {
    await connectPeers({
      munge: true,
      cfgB: { disableFingerprintVerification: true },
    });
    t.pass("connection succeeded with verification disabled");
  } catch (e) {
    t.fail(e);
  }
});

test("fingerprint intact - normal connection succeeds", async (t) => {
  t.plan(1);
  try {
    await connectPeers();
    t.pass("baseline success");
  } catch (e) {
    t.fail(e);
  }
});
