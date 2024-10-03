import React from 'react';
import { RouterProvider } from 'react-router-dom';
import router from './router/router';
import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import { useGlobalMessageHandler } from './hooks/useGlobalMessageHandler';
import  useLog  from './hooks/useLog';
import { ReduxProvider } from './stores/provider';
// 定义路由配置

// 主应用组件
const App = () => {
  if (process.env.BASE_ENV === 'prod') {
    useGlobalMessageHandler();
    useLog();
  }

  return (
    <ReduxProvider>
      <ConfigProvider locale={zhCN}>
        <RouterProvider router={router} />
      </ConfigProvider>
    </ReduxProvider>
  );
};

export default App;
