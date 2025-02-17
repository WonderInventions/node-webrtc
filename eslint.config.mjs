import globals from "globals";
import path from "node:path";
import { fileURLToPath } from "node:url";
import js from "@eslint/js";
import { FlatCompat } from "@eslint/eslintrc";
import eslintConfigPrettier from "eslint-config-prettier";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const compat = new FlatCompat({
  baseDirectory: __dirname,
  recommendedConfig: js.configs.recommended,
  allConfig: js.configs.all,
});

export default [
  ...compat.extends("eslint:recommended"),
  {
    languageOptions: {
      globals: {
        ...globals.node,
        ...globals.browser,
      },

      ecmaVersion: 2024,
      sourceType: "commonjs",
    },

    rules: {
      "array-bracket-spacing": 2,
      "block-spacing": 2,
      "callback-return": 0,
      camelcase: 2,
      "comma-spacing": 2,
      "comma-style": [2, "last"],
      "computed-property-spacing": 2,
      "consistent-return": 1,
      "consistent-this": [2, "self"],
      "dot-location": [2, "property"],
      "dot-notation": 2,
      "eol-last": 2,
      eqeqeq: 2,
      "func-style": [2, "declaration"],
      "handle-callback-err": 2,
      "key-spacing": 2,
      "linebreak-style": [2, "unix"],
      "new-cap": 2,
      "new-parens": 2,
      "no-alert": 2,
      "no-caller": 2,
      "no-console": 1,
      "no-const-assign": 2,
      "no-else-return": 2,
      "no-eval": 2,
      "no-extend-native": 2,
      "no-extra-bind": 2,
      "no-implicit-coercion": 0,
      "no-implied-eval": 2,
      "no-invalid-this": 2,
      "no-iterator": 2,
      "no-labels": 2,
      "no-lone-blocks": 2,
      "no-lonely-if": 2,
      "no-loop-func": 2,
      "no-multiple-empty-lines": 2,
      "no-nested-ternary": 0,
      "no-new-object": 2,
      "no-new-require": 2,
      "no-path-concat": 2,
      "no-process-exit": 2,
      "no-proto": 2,
      "no-return-assign": 2,
      "no-shadow": 0,
      "no-shadow-restricted-names": 2,
      "no-script-url": 2,
      "no-self-compare": 2,
      "no-sequences": 2,
      "no-spaced-func": 2,
      "no-throw-literal": 2,
      "no-trailing-spaces": 2,
      "no-undef-init": 2,
      "no-undefined": 2,
      "no-unneeded-ternary": 2,
      "no-unused-expressions": 2,
      "no-use-before-define": [2, "nofunc"],
      "no-useless-call": 2,
      "no-useless-concat": 2,
      "no-with": 1,
      "object-curly-spacing": [2, "always"],
      "one-var": [2, "never"],
      "operator-assignment": [2, "always"],
      "quote-props": [2, "consistent"],
      semi: [2, "always"],
      "keyword-spacing": 2,
      "space-before-blocks": [2, "always"],

      "space-before-function-paren": [
        2,
        {
          anonymous: "never",
          named: "never",
          asyncArrow: "always",
        },
      ],

      "space-infix-ops": 2,
      "space-unary-ops": 2,
      "spaced-comment": 1,
      strict: [2, "global"],
    },
  },
  eslintConfigPrettier,
];
