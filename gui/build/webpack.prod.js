const path = require("path");
const webpack = require("webpack");
const CopyPlugin = require("copy-webpack-plugin");
const { merge } = require("webpack-merge");
const baseConfig = require("./webpack.base.js");
const CssMinimizerPlugin = require("css-minimizer-webpack-plugin"); // 压缩css文件
const TerserPlugin = require("terser-webpack-plugin"); // 压缩js
const globAll = require("glob-all");
const { PurgeCSSPlugin } = require("purgecss-webpack-plugin"); // // 清理无用css
const CompressionPlugin = require("compression-webpack-plugin");
const { v4: uuidv4 } = require("uuid");

module.exports = merge(baseConfig, {
  mode: "production", // 生产模式,会开启tree-shaking和压缩代码,以及其他优化
  optimization: {
    usedExports: true, // tree-shaking
    minimizer: [
      new CssMinimizerPlugin(), // 压缩css
      new TerserPlugin({
        // 压缩js
        parallel: true, // 开启多线程压缩
        terserOptions: {
          // 压缩配置
          compress: {
            dead_code: true, // 删除无用代码
          },
        },
      }),
    ],
    splitChunks: {
      // 分隔代码
      cacheGroups: {
        vendors: {
          // 提取node_modules代码
          test: /node_modules/, // 只匹配node_modules里面的模块
          name: "vendors", // 提取文件命名为vendors,js后缀和chunkhash会自动加
          minChunks: 1, // 只要使用一次就提取出来
          chunks: "initial", // 只提取初始化就能获取到的模块,不管异步的
          minSize: 0, // 提取代码体积大于0就提取出来
          priority: 1, // 提取优先级为1
        },
        commons: {
          // 提取页面公共代码
          name: "commons", // 提取文件命名为commons
          minChunks: 2, // 只要使用两次就提取出来
          chunks: "initial", // 只提取初始化就能获取到的模块,不管异步的
          minSize: 0, // 提取代码体积大于0就提取出来
        },
      },
    },
  },
  plugins: [
    // 复制文件插件
    new CopyPlugin({
      patterns: [
        {
          from: path.resolve(__dirname, "../public"), // 复制public下文件
          to: path.resolve(__dirname, "../dist"), // 复制到dist目录中
          filter: source => {
            return !source.includes("index.html"); // 忽略index.html
          },
        },
      ],
    }),
    // 去除没用到的css插件
    new PurgeCSSPlugin({
      // 检测src下所有tsx文件和public下index.html中使用的类名和id和标签名称
      // 只打包这些文件中用到的样式
      paths: globAll.sync([
        `${path.join(__dirname, "../src")}/**/*.tsx`,
        `${path.join(__dirname, "../public")}/index.html`,
      ]),
      safelist: {
          standard: [/^ant-/, /^hover:/, /^focus:/, /^active:/, /^disabled:/, /^module_/, /^fade/],
      },
    }),
    new CompressionPlugin({
      test: /.(js|css)$/, // 只生成css,js压缩文件
      filename: "[path][base].gz", // 文件命名
      algorithm: "gzip", // 压缩格式,默认是gzip
      threshold: 10240, // 只有大小大于该值的资源会被处理。默认值是 10k
      minRatio: 0.8, // 压缩率,默认值是 0.8
    }),
    new webpack.DefinePlugin({
      "zipfiles": JSON.stringify({
        NODE_ENV: "production",
        BASE_ENV: process.env.BASE_ENV,
        BUILD_ID: uuidv4(),
        BUILD_TIME: new Date().toLocaleString(),
      }),
    }),
  ],
  resolve: {
    alias: {
      "@useApi": path.join(__dirname, "../src/hooks/useApi/production"), 
      "@useEntry": path.join(__dirname, "../src/prod.tsx"),
    },
  },
});
