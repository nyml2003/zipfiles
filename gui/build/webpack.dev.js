const path = require("path");
const { merge } = require("webpack-merge");
const webpack = require("webpack");
const baseConfig = require("./webpack.base.js");
// 在不需要刷新浏览器的前提下模块热更新,并且能够保留react组件的状态。
const ReactRefreshWebpackPlugin = require("@pmmmwh/react-refresh-webpack-plugin");

// 合并公共配置,并添加开发环境配置
module.exports = merge(baseConfig, {
  mode: "development", // 开发模式,打包更加快速,省了代码优化步骤
  devtool: "eval-cheap-module-source-map", // 源码调试模式,后面会讲
  plugins: [
    new ReactRefreshWebpackPlugin(), // 添加热更新插件
    new webpack.DefinePlugin({
      "zipfiles": JSON.stringify({
        BASE_ENV: process.env.BASE_ENV,
        NODE_ENV: "development",
      }),
    }),
  ],
  devServer: {
    open: true, // 自动打开浏览器
    port: 3000, // 服务端口号
    compress: false, // gzip压缩,开发环境不开启,提升热更新速度
    hot: true, // 开启热更新，后面会讲react模块热替换具体配置
    historyApiFallback: true, // 解决history路由404问题
    static: {
      directory: path.join(__dirname, "../public"), // 托管静态资源public文件夹
    },
    client: {
      // 当出现编译器错误或警告时，就在网页上显示一层黑色的背景层和错误信息
      overlay: true,
      progress: true, // 编译的进度条
    },
  },
  resolve: {
    alias: {
      "@useApi": path.join(__dirname, "../src/hooks/useApi/development"),
      "@useEntry": path.join(__dirname, "../src/dev.tsx"),
    },
  },
});
