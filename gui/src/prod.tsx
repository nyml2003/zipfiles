import React from 'react';
import { RouterProvider } from 'react-router-dom';
import router from './router/router';
import { ConfigProvider } from 'antd';
import zhCN from 'antd/locale/zh_CN';

import { ReduxProvider } from './stores/provider';
import { GlobalProvider } from '@/contexts/globalContext';

const App = () => {
  return (
    <ReduxProvider>
      <GlobalProvider>
        <ConfigProvider locale={zhCN}>
          <RouterProvider router={router} />
        </ConfigProvider>
      </GlobalProvider>
    </ReduxProvider>
  );
};

export default App;
