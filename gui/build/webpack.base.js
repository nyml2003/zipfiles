/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-15 23:05:22
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-18 01:36:54
 * @FilePath: \react\webpack5-ts-react18\build\webpack.base.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
const path = require('path')
const HtmlWebpackPlugin = require('html-webpack-plugin')
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
const webpack = require('webpack')
const WebpackBar = require('webpackbar')
const FriendlyErrorsWebpackPlugin = require('@nuxt/friendly-errors-webpack-plugin')

const env = require('./env.js')
const isDev = process.env.NODE_ENV === 'development' // // 是否是开发模式

// 获取自定义变量集
const oriEnv = env[process.env.BASE_ENV]
Object.assign(oriEnv, {
  BASE_ENV: process.env.BASE_ENV,
})
const defineEnv = {}
for (const key in oriEnv) {
  defineEnv[`process.env.${key}`] = JSON.stringify(oriEnv[key])
}

module.exports = {
  entry: {
    index: path.join(__dirname, '../src/index.tsx'),
    // 如果有第二个页面则配置第二个入口
    // app: [resolve('src/app/index.tsx')],
  }, // 入口文件
  // 打包文件出口
  output: {
    filename: 'static/js/[name].[contenthash].js', // 每个输出js的名称
    path: path.join(__dirname, '../dist'), // 打包结果输出路径
    clean: true, // webpack4需要配置clean-webpack-plugin来删除dist文件,webpack5内置了
    publicPath: isDev ? '/' : './', // 打包后文件的公共前缀路径
  },
  // 开启webpack持久化存储缓存
  cache: {
    type: 'filesystem', // 使用文件缓存
  },
  module: {
    rules: [
      {
        test: /\.css$/, //匹配所有的 css 文件
        enforce: 'pre',
        use: [
          isDev ? 'style-loader' : MiniCssExtractPlugin.loader, // // 开发环境使用style-looader,打包模式抽离css
          'css-loader',
          'postcss-loader',
        ],
      },
      {
        test: /\.less$/, //匹配所有的 less 文件
        enforce: 'pre',
        include: [path.resolve(__dirname, '../src')],
        use: [
          isDev ? 'style-loader' : MiniCssExtractPlugin.loader,
          'css-loader',
          'postcss-loader',
          // 'less-loader'
          // 如果使用antd，引入less@import '~antd/dist/antd.less'; 修改less-loader
          {
            loader: 'less-loader',
            options: {
              lessOptions: {
                javascriptEnabled: true,
              },
            },
          },
        ],
      },
      {
        test: /\.(ts|tsx)$/,
        include: [path.resolve(__dirname, '../src')],
        enforce: 'pre',
        use: [
          {
            loader: 'thread-loader',
            options: {
              workers: 3, // 开启几个 worker 进程来处理打包，默认是 os.cpus().length - 1
            },
          },
          'babel-loader',
        ],
      },
      {
        test: /\.(svg)$/, // 匹配svg文件
        use: ['@svgr/webpack'],
      },
      {
        test: /\.(png|jpg|jpeg|gif)$/, // 匹配图片文件
        type: 'asset', // type选择asset
        parser: {
          //转base64的条件
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: 'static/images/[name].[contenthash:6][ext]',
        },
      },
      {
        test: /.(woff2?|eot|ttf|otf)$/, // 匹配字体图标文件
        type: 'asset', // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: 'static/fonts/[name].[contenthash:6][ext]', // 文件输出目录和命名
        },
      },
      {
        test: /.(mp4|webm|ogg|mp3|wav|flac|aac)$/, // 匹配媒体文件
        type: 'asset', // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: 'static/media/[name].[contenthash:6][ext]', // 文件输出目录和命名
        },
      },
    ],
  },
  resolve: {
    alias: {
      '@': path.join(__dirname, '../src'),
    },
    extensions: ['.js', '.tsx', '.ts'], //注意把高频出现的文件后缀放在前面
  },
  plugins: [
    new HtmlWebpackPlugin({
      title: 'webpack5-ts-react18',
      template: path.resolve(__dirname, '../public/index.html'), // 模板取定义root节点的模板
      inject: true, // 自动注入静态资源
      filename: 'index.html',
    }),
    new webpack.DefinePlugin(defineEnv),
    // 进度条
    new WebpackBar({
      name: 'webpack5-ts-react18',
      // react 蓝
      color: '#61dafb',
      basic: false, // 默认true，启用一个简单的日志报告器
      profile: false, // 默认false，启用探查器。
    }),
    // 优化控制台输出界面
    new FriendlyErrorsWebpackPlugin({
      compilationSuccessInfo: {
        messages: ['You application is running here http://localhost:3000'],
        notes: ['Some additional notes to be displayed upon successful compilation'],
      },
    }),
  ],
  stats: 'errors-only',
}
