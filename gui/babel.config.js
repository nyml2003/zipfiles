/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-16 00:54:57
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-16 01:25:33
 * @FilePath: \react\webpack5-ts-react18\babel.config.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

const selectEnv = require('./build/utils');

module.exports = {
  // 执行顺序由右往左,所以先处理ts,再处理jsx,最后再试一下babel转换为低版本语法
  presets: [
    [
      '@babel/preset-env',
      {
        // 设置兼容目标浏览器版本,这里可以不写,babel-loader会自动寻找上面配置好的文件.browserslistrc
        // "targets": {
        //  "chrome": 35,
        //  "ie": 9
        // },
        modules: false, // 不转换模块语法,保留原本的es6模块语法
        useBuiltIns: 'usage', // 根据配置的浏览器兼容,以及代码中使用到的api进行引入polyfill按需添加
        corejs: 3, // 配置使用core-js使用的版本
      },
    ],
    '@babel/preset-react',
    '@babel/preset-typescript',
  ],
  plugins: [
    // 支持装饰器语法
    ['@babel/plugin-proposal-decorators', { legacy: true }],
    '@babel/plugin-syntax-dynamic-import', // 支持动态导入语法
    selectEnv(require.resolve('react-refresh/babel'), ''), // 根据环境选择是否启动react热更新插件
  ].filter(Boolean), // 过滤空值
};
