import { useGlobalMessageHandler } from '@/hooks/useGlobalMessageHandler';
import useLog from '@/hooks/useLog';
import React, { createContext, useContext, ReactNode } from 'react';

// 创建 Context
const GlobalContext = createContext(null);

// Context Provider 组件
export const GlobalProvider = ({ children }: { children: ReactNode }) => {
  const globalMessageHandler = useGlobalMessageHandler();
  const log = useLog();

  // 这里可以添加更多的全局状态和副作用

  return <GlobalContext.Provider value={null}>{children}</GlobalContext.Provider>;
};

// 使用 Context
export const useGlobal = () => useContext(GlobalContext);
