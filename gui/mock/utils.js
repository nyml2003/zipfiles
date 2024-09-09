/* eslint-disable @typescript-eslint/no-var-requires */
/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-26 20:58:54
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-26 20:59:10
 * @FilePath: \react\webpack5-ts-react18\mock\utils.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
const { join } = require('path')
const fs = require('fs')

/**
 * @description 随机生成图片url。
 * @param width
 * @param height
 * @returns {string}
 */
// function handleRandomImage(width = 50, height = 50) {
//   return `https://picsum.photos/${width}/${height}?random=${Random.guid()}`
// }

/**
 * @description 处理所有 controller 模块，npm run serve时在node环境中自动输出controller文件夹下Mock接口，请勿修改。
 * @returns {[]}
 */
function handleMockArray() {
  const mockArray = []
  const getFiles = (jsonPath) => {
    const jsonFiles = []
    const findJsonFile = (path) => {
      const files = fs.readdirSync(path)
      files.forEach((item) => {
        const fPath = join(path, item)
        const stat = fs.statSync(fPath)
        if (stat.isDirectory() === true) findJsonFile(item)
        if (stat.isFile() === true) jsonFiles.push(item)
      })
    }
    findJsonFile(jsonPath)
    jsonFiles.forEach((item) => mockArray.push(`./controller/${item}`))
  }
  getFiles('mock/controller')
  return mockArray
}
module.exports = {
  // handleRandomImage,
  handleMockArray
}
