import React, { useEffect } from 'react';
import { RouterProvider } from 'react-router-dom';
import router from './router';
import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';
import { useGlobalMessageHandler } from './hooks/useGlobalMessageHandler';
import useApi from './hooks/useApi';
import { LogRequest } from './apis/log';
import { ApiEnum } from './apis';
// 定义路由配置

// 主应用组件
const App = () => {
  useGlobalMessageHandler();

  useEffect(() => {
    if (process.env.BASE_ENV === 'prod') {
      const api = useApi();
      if (process.env.BASE_ENV === 'prod') {
        console.log = new Proxy(console.log, {
          apply: (target, thisArg, argArray) => {
            api.call(ApiEnum.Log, { message: argArray.join(' ') } as LogRequest);
            target(...argArray);
          },
        });
        console.error = new Proxy(console.error, {
          apply: (target, thisArg, argArray) => {
            api.call(ApiEnum.Log, { message: argArray.join(' ') } as LogRequest);
            target(...argArray);
          },
        });
        console.warn = new Proxy(console.warn, {
          apply: (target, thisArg, argArray) => {
            api.call(ApiEnum.Log, { message: argArray.join(' ') } as LogRequest);
            target(...argArray);
          },
        });
        console.info = new Proxy(console.info, {
          apply: (target, thisArg, argArray) => {
            api.call(ApiEnum.Log, { message: argArray.join(' ') } as LogRequest);
            target(...argArray);
          },
        });
      }
    }
  }, []);

  return (
    <ConfigProvider locale={zhCN}>
      <RouterProvider router={router} />
    </ConfigProvider>
  );
};

export default App;
