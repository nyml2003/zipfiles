const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const webpack = require("webpack");
const WebpackBar = require("webpackbar");
const FriendlyErrorsWebpackPlugin = require("@nuxt/friendly-errors-webpack-plugin");
const selectEnv = require("./utils");

module.exports = {
  entry: {
    index: path.join(__dirname, "../src/index.tsx"),
    // 如果有第二个页面则配置第二个入口
    // app: [resolve('src/app/index.tsx')],
  }, // 入口文件
  // 打包文件出口
  output: {
    filename: "static/js/[name].[contenthash].js", // 每个输出js的名称
    path: path.join(__dirname, "../dist"), // 打包结果输出路径
    clean: true, // webpack4需要配置clean-webpack-plugin来删除dist文件,webpack5内置了
    publicPath: selectEnv("/", "./"), // 根据环境选择路径
  },
  // 开启webpack持久化存储缓存
  cache: {
    type: "filesystem", // 使用文件缓存
  },
  module: {
    rules: [
      {
        test: /\.module\.css$/,
        enforce: "pre",
        include: [path.resolve(__dirname, "../src")],
        use: [
          selectEnv("style-loader", MiniCssExtractPlugin.loader),
          {
            loader: "css-loader",
            options: {
              modules: {
                localIdentName: "module_[local]_[hash:base64:6]",
              },
            },
          },
          "postcss-loader",
        ],
      },
      {
        test: /\.css$/, // 匹配所有的 css 文件
        exclude: /\.module\.css$/,
        enforce: "pre",
        use: [
          selectEnv("style-loader", MiniCssExtractPlugin.loader),
          "css-loader",
          "postcss-loader",
        ],
      },
      {
        test: /\.module\.less$/,
        enforce: "pre",
        include: [path.resolve(__dirname, "../src")],
        use: [
          selectEnv("style-loader", MiniCssExtractPlugin.loader),
          {
            loader: "css-loader",
            options: {
              modules: {
                localIdentName: "module_[local]_[hash:base64:6]",
              },
            },
          },
          "postcss-loader",
          {
            loader: "less-loader",
            options: {
              lessOptions: {
                javascriptEnabled: true,
              },
            },
          },
        ],
      },
      {
        test: /\.less$/, // 匹配所有的 less 文件
        exclude: /\.module\.less$/,
        enforce: "pre",
        include: [path.resolve(__dirname, "../src")],
        use: [
          selectEnv("style-loader", MiniCssExtractPlugin.loader),
          "css-loader",
          "postcss-loader",
          {
            loader: "less-loader",
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
        include: [path.resolve(__dirname, "../src")],
        enforce: "pre",
        use: [
          {
            loader: "thread-loader",
            options: {
              workers: 7, // 开启几个 worker 进程来处理打包，默认是 os.cpus().length - 1
            },
          },
          "babel-loader",
        ],
      },
      {
        test: /\.(svg)$/, // 匹配svg文件
        use: ["@svgr/webpack"],
      },
      {
        test: /\.(png|jpg|jpeg|gif)$/, // 匹配图片文件
        type: "asset", // type选择asset
        parser: {
          // 转base64的条件
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: "static/images/[name].[contenthash:6][ext]",
        },
      },
      {
        test: /.(woff2?|eot|ttf|otf)$/, // 匹配字体图标文件
        type: "asset", // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: "static/fonts/[name].[contenthash:6][ext]", // 文件输出目录和命名
        },
      },
      {
        test: /.(mp4|webm|ogg|mp3|wav|flac|aac)$/, // 匹配媒体文件
        type: "asset", // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          },
        },
        generator: {
          filename: "static/media/[name].[contenthash:6][ext]", // 文件输出目录和命名
        },
      },
    ],
  },
  optimization: {
    splitChunks: {
      chunks: "all",
    },
  },
  resolve: {
    alias: {
      "@": path.join(__dirname, "../src"),
    },
    extensions: [".js", ".tsx", ".ts"], // 注意把高频出现的文件后缀放在前面
  },
  plugins: [
    new HtmlWebpackPlugin({
      title: "zipfiles",
      template: path.resolve(__dirname, "../public/index.html"), // 模板取定义root节点的模板
      inject: true, // 自动注入静态资源
      filename: "index.html",
    }),
    new webpack.DefinePlugin({
      "process.env": JSON.stringify({
        BASE_ENV: process.env.BASE_ENV,
      }),
    }),
    // 进度条
    new WebpackBar({
      name: "zipfiles",
      // react 蓝
      color: "#61dafb",
      basic: false, // 默认true，启用一个简单的日志报告器
      profile: false, // 默认false，启用探查器。
    }),
    // 优化控制台输出界面
    new FriendlyErrorsWebpackPlugin({
      compilationSuccessInfo: {
        messages: ["You application is running here http://localhost:3000"],
        notes: ["Some additional notes to be displayed upon successful compilation"],
      },
    }),
  ],
  stats: "errors-only",
};
