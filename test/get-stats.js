"use strict";

const test = require("tape");

const { RTCPeerConnection } = require("..");

function settleWithin(promise, ms) {
  let timer;
  return Promise.race([
    promise.then(
      (value) => ({ status: "resolved", value }),
      (reason) => ({ status: "rejected", reason }),
    ),
    new Promise((resolve) => {
      timer = setTimeout(() => resolve({ status: "pending" }), ms);
    }),
  ]).finally(() => clearTimeout(timer));
}

test("getStats() resolves when close() is called while it is pending", async (t) => {
  // Whether the stats are still pending when close() runs is up to libwebrtc's
  // threads, so repeat to catch it.
  for (let attempt = 1; attempt <= 50; attempt++) {
    const pc = new RTCPeerConnection();
    try {
      pc.createDataChannel("data");
      await pc.setLocalDescription(await pc.createOffer());
      const stats = pc.getStats();
      pc.close();
      const outcome = await settleWithin(stats, 2000);
      if (outcome.status !== "resolved") {
        t.fail(
          `attempt ${attempt}: getStats() ${outcome.status} ${outcome.reason ?? ""}`,
        );
        return;
      }
      if (!(outcome.value instanceof Map) || outcome.value.size === 0) {
        t.fail(`attempt ${attempt}: resolved without a stats report`);
        return;
      }
    } finally {
      pc.close();
    }
  }
  t.pass("every getStats() resolved with a stats report");
});
