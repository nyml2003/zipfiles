/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-15 22:51:13
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-15 22:51:20
 * @FilePath: \react\webpack5-ts-react18\src\index.tsx
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
import React, { useEffect } from 'react'
import { createRoot } from 'react-dom/client'
import App from './App'
import { ApiEnum } from './apis'
import useApi from './hooks/useApi'
const RootComponent = () => {
  const api = useApi()

  useEffect(() => {
    const originalLog = console.log
    console.log = new Proxy(console.log, {
      apply: function (target, thisArg, argumentsList) {
        api.request(ApiEnum.Log, { message: argumentsList.join(' ') }).catch((error) => {
          alert('Error: ' + error)
        })
        // 调用原始 console.log 方法
        return target.apply(thisArg, argumentsList)
      },
    })

    // 在组件卸载时恢复原始的 console.log 方法
    return () => {
      console.log = originalLog
    }
  }, [api])

  return <App />
}

const root = document.getElementById('root')
if (root) {
  createRoot(root).render(<RootComponent />)
}
