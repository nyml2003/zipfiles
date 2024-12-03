import { useGlobalMessageHandler } from "@/hooks/useGlobalMessageHandler";
import useLog from "@/hooks/useLog";
import React, { createContext, ReactNode } from "react";

// 创建 Context
const GlobalContext = createContext(null);

// Context Provider 组件
export const GlobalProvider = ({ children }: { children: ReactNode }) => {
  useGlobalMessageHandler();
  useLog();

  // 这里可以添加更多的全局状态和副作用

  return <GlobalContext.Provider value={null}>{children}</GlobalContext.Provider>;
};
