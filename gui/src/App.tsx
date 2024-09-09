import React from 'react'
import './app.less'
import { useEffect, useState } from 'react'
import { ApiEnum } from './apis'
import useApi from './hooks/useApi'
import { AddRequest } from './apis/add'
const App: React.FC = () => {
  const api = useApi()
  const [counter, setCounter] = useState(0)
  const [result, setResult] = useState(0)

  const handleLogMessage = () => {
    console.log('这是一个日志消息2')
  }

  const handleErrorMessage = () => {
    console.log('这是一个错误消息2')
  }

  const handleCount = () => {
    console.log('counter:', counter)
    const message = {
      args: [counter, 1],
    }

    api
      .request<AddRequest(ApiEnum.add, message)
      .then(result => {
        setResult(result)
        setCounter(counter + 1)
      })
      .catch(error => {
        setResult(error)
      })
  }
  return (
    <div className='flex flex-col h-screen'>
      <header className='flex justify-center items-center h-20 bg-blue-400 text-white text-2xl'>
        这是页头
      </header>
      <div className='flex-1 bg-gray-200 text-2xl text-center'>
        <div className='flex justify-center items-center  align-middle h-full font-jetbrains-mono'>
          <div className='flex flex-col'>
            <div className='flex justify-center items-center h-20'>
              <button className='px-4 py-2 bg-blue-400 text-white' onClick={handleLogMessage}>
                发送日志消息
              </button>
            </div>
            <div className='flex justify-center items-center h-20'>
              <button className='px-4 py-2 bg-blue-400 text-white' onClick={handleErrorMessage}>
                发送错误消息
              </button>
            </div>
            <div className='flex justify-center items-center h-20'>
              <button className='px-4 py-2 bg-blue-400 text-white' onClick={handleCount}>
                计数
              </button>
            </div>
            <div className='flex justify-center items-center h-20'>
              <div>计数器: {counter}</div>
            </div>
            <div className='flex justify-center items-center h-20'>
              <div>结果: {result}</div>
            </div>
          </div>
        </div>
      </div>
      <footer className='flex justify-center items-center h-20 bg-blue-400 text-white text-2xl'>
        这是页脚
      </footer>
    </div>
  )
}
export default App
