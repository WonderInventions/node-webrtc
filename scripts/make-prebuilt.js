"use strict";

// TODO(jack): error handling

const child_process = require("child_process");
const fs = require("fs");
const { platform, arch, buildFolder } = require("./build-vars.js");

// Run a fresh build
child_process.execSync("npm run build", { stdio: "inherit" });

// Copy the resulting binary to the output folder
const input_folder = `${buildFolder}/Release`;
const output_folder = `prebuilds/${platform}-${arch}`;
fs.copyFileSync(`${input_folder}/wrtc.node`, `${output_folder}/wrtc.node`);

// Copy version from main package.json to sub package.json
const main_package_filename = "package.json";
const sub_package_filename = `${output_folder}/package.json`;
const main_package = require(`../${main_package_filename}`);
const sub_package = require(`../${sub_package_filename}`);

const version = main_package.version;
main_package.optionalDependencies[`@avahq/wrtc-${platform}-${arch}`] = version;
sub_package.version = version;

fs.writeFileSync(main_package_filename, JSON.stringify(main_package, null, 2));
fs.writeFileSync(sub_package_filename, JSON.stringify(sub_package, null, 2));
