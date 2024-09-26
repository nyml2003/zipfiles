import React from 'react';
import { RouterProvider } from 'react-router-dom';
import router from './router';
import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import useGlobalMessageHandler from './hooks/useGlobalMessageHandler';
// 定义路由配置

// 主应用组件
const App = () => {
  useGlobalMessageHandler();
  return (
    <ConfigProvider locale={zhCN}>
      <RouterProvider router={router} />
    </ConfigProvider>
  );
};

export default App;
