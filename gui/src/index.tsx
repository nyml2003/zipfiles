/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-15 22:51:13
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-15 22:51:20
 * @FilePath: \react\webpack5-ts-react18\src\index.tsx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
import React from 'react'
import { createRoot } from 'react-dom/client'
import App from './App'
const originalLog = console.log
console.log = new Proxy(console.log, {
  apply: function (target, thisArg, argumentsList) {
    // 发送 log 消息到 C++ 代码
    if (window.webkit && window.webkit.messageHandlers && window.webkit.messageHandlers.log) {
      window.webkit.messageHandlers.log.postMessage(argumentsList.join(' '))
    } else {
      console.error('无法找到 log 消息处理程序')
    }
    // 调用原始 console.log 方法
    return target.apply(thisArg, argumentsList)
  },
})
const root = document.getElementById('root')
if (root) {
  createRoot(root).render(<App />)
}

const request = new Promise((resolve, reject) => {

}).then((res) => {
      console.log(res)
}).catch((e) => {
        throw new Error(e)
})
    
window.webkitt.messageHandlers.aaa.postMessage(request)