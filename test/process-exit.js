"use strict";

const { spawnSync } = require("child_process");
const path = require("path");
const test = require("tape");

const repro = path.join(__dirname, "process-exit", "repro.js");

// Each scenario runs in a child process because the bug is a crash during
// environment teardown, after the last line of JavaScript has run.
["track", "closed"].forEach(function (scenario) {
  test(
    "process exits cleanly after a stopped track (" + scenario + ")",
    function (t) {
      const result = spawnSync(process.execPath, [repro], {
        env: Object.assign({}, process.env, { SCENARIO: scenario }),
        encoding: "utf8",
        timeout: 30000,
      });

      t.equal(
        result.stdout.trim(),
        "OK " + scenario,
        "script ran to completion",
      );
      t.equal(
        result.signal,
        null,
        "process was not killed by a signal" +
          (result.signal ? " (got " + result.signal + ")" : ""),
      );
      t.equal(result.status, 0, "exit status is 0");
      if (result.stderr) {
        t.comment(result.stderr.trim());
      }
      t.end();
    },
  );
});
