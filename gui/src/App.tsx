import React from 'react';
import { RouterProvider } from 'react-router-dom';
import router from './router';
import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import useGlobalMessageHandler from './hooks/useGlobalMessageHandler';
import useApi from './hooks/useApi';
import { LogRequest, LogResponse } from './apis/log';
import { ApiEnum } from './apis';
// 定义路由配置

// 主应用组件
const App = () => {
  if (process.env.BASE_ENV === 'prod') {
    const api = useApi();
    useGlobalMessageHandler();
    if (process.env.BASE_ENV === 'prod') {
      console.log = new Proxy(console.log, {
        apply: (target, thisArg, argArray) => {
          api.request<LogRequest, LogResponse>(ApiEnum.Log, {
            message: argArray.join(' '),
          });
          target(...argArray);
        },
      });
    }
  }

  return (
    <ConfigProvider locale={zhCN}>
      <RouterProvider router={router} />
    </ConfigProvider>
  );
};

export default App;
