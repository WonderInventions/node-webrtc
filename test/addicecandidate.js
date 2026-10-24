"use strict";

const test = require("tape");

const { RTCPeerConnection } = require("..");

const offer = {
  type: "offer",
  sdp: `\
v=0
o=- 2327044227424838191 2 IN IP4 127.0.0.1
s=-
t=0 0
a=group:BUNDLE 0
a=msid-semantic: WMS
m=application 9 DTLS/SCTP 5000
c=IN IP4 0.0.0.0
a=ice-ufrag:ZVjA
a=ice-pwd:ySFnYuYDmL6hstu0f3kgG71w
a=ice-options:trickle
a=fingerprint:sha-256 BD:08:3B:BA:84:0F:E7:0B:CD:61:FD:EA:E7:F1:31:62:14:52:43:DB:CB:5D:1C:60:53:0E:E1:7C:87:41:56:FD
a=setup:actpass
a=mid:0
a=sctpmap:5000 webrtc-datachannel 1024
`
    .split("\n")
    .join("\r\n"),
};

const candidate = {
  candidate:
    "candidate:559267639 1 udp 2122267903 ::1 57693 typ host generation 0 ufrag ZVjA network-id 2",
  sdpMid: "0",
  sdpMLineIndex: 0,
};

test("addIceCandidate", (t) => {
  test("has correct queueing behavior", (t) => {
    const pc = new RTCPeerConnection();
    return Promise.all([
      pc.setRemoteDescription(offer),
      pc.addIceCandidate(candidate),
    ])
      .then(() => {
        return pc.addIceCandidate(null);
      })
      .then(() => {
        pc.close();
        t.pass("addIceCandidate worked correctly");
        t.end();
      });
  });

  test("rejects after close()", async (t) => {
    const timers = [];
    function within(promise, ms) {
      return Promise.race([
        promise,
        new Promise((resolve) =>
          timers.push(setTimeout(resolve, ms, "pending")),
        ),
      ]);
    }
    const pc = new RTCPeerConnection();
    try {
      await pc.setRemoteDescription(offer);
      // Wait until the closed signaling state has been handled, which stops
      // the peer connection's event loop.
      const closed = new Promise((resolve) => {
        pc.onsignalingstatechange = () => {
          if (pc.signalingState === "closed") setImmediate(resolve, "closed");
        };
      });
      pc.close();
      t.equal(await within(closed, 2000), "closed");

      const outcome = await within(
        pc.addIceCandidate(candidate).then(
          () => "resolved",
          (error) => error.name,
        ),
        2000,
      );
      t.equal(outcome, "InvalidStateError");
    } finally {
      timers.forEach(clearTimeout);
      pc.close();
    }
  });

  t.end();
});
