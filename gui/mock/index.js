/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-26 20:16:10
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-26 21:01:06
 * @FilePath: \react\webpack5-ts-react18\mock\index.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/* eslint-disable @typescript-eslint/no-var-requires */
const { handleMockArray } = require('./utils')
const mockData = []
const mockArray = handleMockArray()
mockArray.forEach((item) => {
  const obj = require(item)
  mockData.push(...obj)
})
function Mock(app) {
  mockData.forEach(item => {
    app[item['type']](item['url'], (req, res) => {
      res.json(item['response'])
    })
  })
  // app.get('/xxxx/yyy', (req, res) => {
  //   console.log('getPower111')
  //   res.json('getPower')
  // })
  // app.post('/reconfig', (req, res) => {
  //   console.log('reConfig111')
  //   res.json('reConfig')
  // })
}

module.exports = Mock
