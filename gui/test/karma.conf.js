// test/karma.conf.js
const chromeLauncher = require("karma-chrome-launcher");
module.exports = function (config) {
  config.set({
    customLaunchers: {
      ChromeHeadless: {
        base: "Chrome",
        flags: ["--headless", "--disable-gpu", "--remote-debugging-port=9222", "--no-sandbox"]
      }
    },
    frameworks: ["jasmine", "karma-typescript"],
    files: [
      { pattern: "../../src/**/*.ts", included: true }, // 指向src目录
      { pattern: "**/*.spec.ts", included: true } // 指向test目录下的测试文件
    ],
    preprocessors: {
      "**/*.ts": ["karma-typescript"]
    },
    reporters: ["progress", "karma-typescript"],
    browsers: ["ChromeHeadless"],
    singleRun: true,
    concurrency: Infinity,
    karmaTypescriptConfig: {
      tsconfig: "tsconfig.test.json" // 指向同一个test目录下的tsconfig.test.json
    }
  });
};
