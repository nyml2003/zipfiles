import React from 'react'
import { HashRouter as Router, useRoutes } from 'react-router-dom'
import routes from './router'
// 定义路由配置
const AppRoutes = () => {
  return useRoutes(routes)
}

// 主应用组件
const App = () => {
  return (
    <Router>
      <AppRoutes />
    </Router>
  )
}

export default App
