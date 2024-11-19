import React from "react";
import { RouterProvider } from "react-router-dom";
import router from "./router/router";
import { ConfigProvider } from "antd";
import zhCN from "antd/locale/zh_CN";

import { ReduxProvider } from "./stores/provider";

const App = () => {
  return (
    <ReduxProvider>
        <ConfigProvider locale={zhCN}>
          <RouterProvider router={router} />
        </ConfigProvider>
    </ReduxProvider>
  );
};

export default App;
