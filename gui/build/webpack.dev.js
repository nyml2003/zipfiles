/* eslint-disable @typescript-eslint/no-var-requires */
/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-15 23:50:30
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-26 20:15:55
 * @FilePath: \react\webpack5-ts-react18\build\webpack.dev.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// webpack.dev.js
const path = require('path');
const { merge } = require('webpack-merge');
const baseConfig = require('./webpack.base.js');
// 在不需要刷新浏览器的前提下模块热更新,并且能够保留react组件的状态。
const ReactRefreshWebpackPlugin = require('@pmmmwh/react-refresh-webpack-plugin');

// 合并公共配置,并添加开发环境配置
module.exports = merge(baseConfig, {
  mode: 'development', // 开发模式,打包更加快速,省了代码优化步骤
  devtool: 'eval-cheap-module-source-map', // 源码调试模式,后面会讲
  plugins: [
    new ReactRefreshWebpackPlugin(), // 添加热更新插件
  ],
  devServer: {
    open: true, // 自动打开浏览器
    port: 3000, // 服务端口号
    compress: false, // gzip压缩,开发环境不开启,提升热更新速度
    hot: true, // 开启热更新，后面会讲react模块热替换具体配置
    historyApiFallback: true, // 解决history路由404问题
    static: {
      directory: path.join(__dirname, '../public'), // 托管静态资源public文件夹
    },
    client: {
      // 当出现编译器错误或警告时，就在网页上显示一层黑色的背景层和错误信息
      overlay: true,
      progress: true, // 编译的进度条
    },
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    // before (app) {
    //   // do fancy stuff
    //   if (isMock) {
    //     mock(app)
    //   }
    // },
    proxy: {
      '/api': {
        target: 'https://xx.console.xx.com',
        pathRewrite: {
          '^/api': '/assetAnalysis',
        }, // 开头的 /project 路径，会被替换为http://127.0.0.1:8888/路径
        ws: true,
        changeOrigin: true, // 突破网站对爬虫的限制, 一般都要开启
      },
      '/react-api': {
        target: 'https://www.xx.com.cn/xx-api', // 代理 mock 服务的请求, 相当于是 /api 开头的全部匹配到 http://localhost:5000/api
        pathRewrite: {
          '^/react-api': '/',
        },
        ws: true,
        changeOrigin: true,
      },
    },
  },
  resolve: {
    alias: {
      '@useApi': path.join(__dirname, '../src/hooks/useApi/development'),
    },
  },
});
