import useStateProvider from "@/hooks/useStateProvider";
import React, { ReactNode } from "react";

// 路径选择器组件状态
interface State {
  path: string; // 当前路径
  fresh: boolean; // 是否刷新
  file: string; // 文件名
  chosenPath?: string; // 选中的路径
}

// 路径选择器组件的默认状态
const defaultState: State = {
  path: "",
  fresh: false,
  file: ""
};

// 以下为路径选择器组件的reducer函数

const updatePath = (state: State, payload: string) => {
  state.path = payload;
  state.file = defaultState.file;
};

const refresh = (state: State) => {
  state.fresh = !state.fresh;
};

const updateFile = (state: State, payload: string) => {
  state.file = payload;
};

const reducers = {
  updatePath,
  refresh,
  updateFile
};

// 使用useStateProvider推荐的方式创建路径选择器组件的Context和Provider

const { Context, Provider } = useStateProvider(reducers, defaultState);
export const createProvider = (children: ReactNode, initialState?: Partial<State>) => {
  return <Provider initialState={initialState}>{children}</Provider>;
};
export { Context };
