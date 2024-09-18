/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-17 12:29:25
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-17 12:47:28
 * @FilePath: \react\webpack5-ts-react18\build\env.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
// 环境变量值配置
module.exports = {
  'dev': {
    'BASEURL': 'https://baidu.dev.com/api',
    'APP_LOGIN_URL': '//app.xxx.com',
    'APP_BASE_API': '//app.xxx.com',
  },
  'test': {
    'BASEURL': 'https://baidu.test.com/api',
    'APP_LOGIN_URL': '//app.xxx.com',
    'APP_BASE_API': '//app.xxx.com',
  },
  'pre': {
    'BASEURL': 'https://baidu.test.com/api',
    'APP_LOGIN_URL': '//app.xxx.com',
    'APP_BASE_API': '//app.xxx.com',
  },
  'prod': {
    'BASEURL': 'https://baidu.prod.com/api',
    'APP_LOGIN_URL': '//app.xxx.com',
    'APP_BASE_API': '////app.xxx.com',
  }
}
