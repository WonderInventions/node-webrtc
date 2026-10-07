"use strict";

// Standalone reproduction of the segfault at natural process exit.
//
//   node test/process-exit/repro.js; echo "exit $?"
//
// Prints "OK" and then dies with SIGSEGV (exit 139) during Node's environment
// teardown, when the remaining ObjectWrap finalizers run in an unspecified
// order after the addon's cleanup hook has already destroyed the
// PeerConnectionFactory and its threads. Nothing is left running: the track is
// stopped and the peer connection (if any) is closed, so the event loop drains
// and the process exits on its own. process.exit() takes a different path and
// does not crash, which is why the test suite (which ends with process.exit)
// never caught this.
//
// SCENARIO=track    RTCAudioSource -> createTrack -> stop            (default)
// SCENARIO=closed   same, plus a closed RTCPeerConnection and a stopped sink
//
// Output goes through fs.writeSync: console.log on a piped stdout is
// asynchronous, which keeps the event loop alive one more turn and changes
// the finalizer order enough to hide the crash (`node repro.js | cat` exits 0
// while `node repro.js > /dev/null` segfaults).

const fs = require("fs");
const wrtc = require("../..");

const { RTCAudioSource, RTCAudioSink } = wrtc.nonstandard;
const scenario = process.env.SCENARIO || "track";

async function main() {
  const source = new RTCAudioSource();
  const track = source.createTrack();

  if (scenario === "closed") {
    const pc = new wrtc.RTCPeerConnection();
    pc.addTrack(track);
    const sink = new RTCAudioSink(track);
    sink.stop();
    pc.close();
  }

  track.stop();
  fs.writeSync(1, "OK " + scenario + "\n");
}

main();
