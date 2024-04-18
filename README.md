<h1 align="center">
  <img height="120px" src="https://upload.wikimedia.org/wikipedia/commons/d/d9/Node.js_logo.svg">&nbsp;&nbsp;&nbsp;&nbsp;
  <img height="120px" src="https://webrtc.github.io/webrtc-org/assets/images/webrtc-logo-vert-retro-dist.svg">
</h1>

[![NPM](https://img.shields.io/npm/v/@avahq/wrtc.svg)](https://www.npmjs.com/package/@avahq/wrtc)

node-webrtc is a Node.js Native Addon that provides bindings to [WebRTC
M94](https://webrtc.googlesource.com/src/+/branch-heads/4606). This project is
aiming for spec-compliance and will eventually be tested using the W3C's
[web-platform-tests](https://github.com/web-platform-tests/wpt) project. A
number of [nonstandard APIs](docs/nonstandard-apis.md) for testing are also
included.

## Install

```
npm install @avahq/wrtc
```

Installing from NPM downloads a prebuilt binary for your operating system ×
architecture, based on optional dependency filters.

To install a debug build or cross-compile, you should [build from
source](docs/build-from-source.md).

## Supported Platforms

The following platforms are confirmed to work with node-webrtc and have
prebuilt binaries available. Since node-webrtc targets [N-API version
3](https://nodejs.org/api/n-api.html), there may be additional platforms
supported that are not listed here. If your platform is not supported, you may
still be able to [build from source](docs/build-from-source.md).

<table>
  <thead>
    <tr>
      <td colspan="2" rowspan="2"></td>
      <th colspan="2">Linux</th>
      <th colspan="2">macOS</th>
      <th>Windows</th>
    </tr>
    <tr>
      <th>x64</th>
      <th>arm64</th>
      <th>x64</th>
      <th>arm64</th>
      <th>x64</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2">Node</th>
      <th>18</th>
      <td align="center">✓</td>
      <td align="center">?</td>
      <td align="center">✓</td>
      <td align="center">✓</td>
      <td align="center">✓</td>
    </tr>
    <tr>
      <th>20</th>
      <td align="center">✓</td>
      <td align="center">✓</td>
      <td align="center">✓</td>
      <td align="center">✓</td>
      <td align="center">✓</td>
    </tr>
  </tbody>
</table>

## Examples

See [node-webrtc/node-webrtc-examples](https://github.com/node-webrtc/node-webrtc-examples).

## Build @avahq module and deploy them

- first login via npm login
- the compile the binary via dockerfile `docker build -t webrtc-forked/arm64 .  || docker build -t webrtc-forked/amd64 -f Dockerfile.amd64 .` if needed our your personnal machine, use the target arch via `TARGET_ARCH=arm64|x64 npm run build`
- It can take time to build (~15mn on mac m2)
- Run the container image you build
- Then get the `wrtc.node` from the `build-${arch}` folder (via docker cp or docker file dashboard) and put it inside the right `prebuild/${arch}` folder
- Do it for all platform/arch you can
- Run `npm version major|minor|patch..` inside each subfolder
- Run `npm publish` and wait for ok return (if e422 please add --access-public to the publish command)
- Update the version linked for every submodules into `package.json` here
- Run `npm version major|minor|patch..` inside this project
- Run `npm publish` inside this project
- DONE
