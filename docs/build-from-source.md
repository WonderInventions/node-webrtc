# Build from Source

## Prerequisites

node-webrtc uses [cmake-js](https://github.com/cmake-js/cmake-js) to build
from source. When building from source, in addition to the prerequisites
required by cmake-js, you will need

- Git
- Ninja
- CMake 3.15 or newer
- Linux: GCC 10.1 or newer
- MacOS: Xcode 12 or newer
  - MacOSX11.3.sdk via git clone https://github.com/phracker/MacOSX-SDKs
  - linked via `sudo ln -s /Users/path/to/MacOSX-SDKs/MacOSX11.3.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk`
- Windows: Microsoft Visual Studio 2022 or newer, with the Clang toolchain installed
- Check the [additional prerequisites listed by WebRTC](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/) - although their install is automated by the CMake scripts provided

## Install

Once you have the prerequisites, clone the repository, set the `SKIP_DOWNLOAD`
environment variable to "true", and run `npm install`. Just like when
installing prebuilt binaries, you can set the `TARGET_ARCH` environment
variable to "arm" or "arm64" to build for armv7l or arm64, respectively. Linux
and macOS users can also set the `DEBUG` environment variable for debug builds.

```
git clone https://github.com/node-webrtc/node-webrtc.git
cd node-webrtc
npm run build
```

## Subsequent Builds

Subsequent builds can be triggered with `cmake`, e.g. on MacOS:

```
cmake --build build-darwin-x64
```

You can pass either `--debug` or `--release` to build a debug or release build
of node-webrtc (and the underlying WebRTC library). Refer to the CMake
documentation for additional command line options.

## Other Notes

### Linux

On Linux, we dynamically link against the platform's libc and libc++.
Also, although we compile WebRTC sources with Clang (downloaded as part of
WebRTC's build process), we compile node-webrtc sources with the platform's
complier

### macOS

On macOS, we statically link libc++ and libc++abi. Also, we compile WebRTC
sources with the version of Clang downloaded as part of WebRTC's build process,
but we compile node-webrtc sources using the system Clang.

#### arm64

In order to cross-compile for arm64 on MacOS,

1. Set `TARGET_ARCH` to "arm64"
2. Re-run `npm run build`

### Windows

We use the Clang toolchain and the Ninja generator on Windows in order to have
similar support for the `clangd` language server and `compile_commands.json`;
Visual Studio proper has not been tested.

To fix the error `Filename too long`, when downloading libwebrtc, use
(optionally with `--global` or `--system` switches to set for more than just
this project):

```
git config core.longpaths true
```

Creating symbolic links with MKLINK is used by the build script but is disabled
for non-Administrative users by default with a local security policy. On
Windows 10, fix this with Run (Windows-R) then `gpedit.msc`. Edit key "Local
Computer Policy -> Windows Settings -> Security Settings -> Local Policies ->
User Rights Assignment -> Create Symbolic Links" and add your user name. Log
out and in to change the policy. Note the [associated security
vunerability](https://docs.microsoft.com/en-us/windows/security/threat-protection/security-policy-settings/create-symbolic-links#vulnerability).

The Windows SDK debugging tools should be installed. One way to achieve this is
to [Download the Windows Driver
Kit](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk).

# Test

## Unit & Integration Tests

Once everything is built, run

```
npm test
```

Other tests will be written as needed, once feature parity is reached again.
