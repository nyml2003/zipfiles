import useStateProvider from "@/hooks/useStateProvider";
import React, { ReactNode } from "react";

interface State {
  path: string; // 当前路径
  fresh: boolean; // 是否刷新
  file: string; // 文件名
  chosenPath?: string; // 选中的路径
}

const defaultState: State = {
  path: "",
  fresh: false,
  file: ""
};

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

const { Context, Provider } = useStateProvider(reducers, defaultState);
export const createProvider = (children: ReactNode, initialState?: Partial<State>) => {
  return <Provider initialState={initialState}>{children}</Provider>;
};
export { Context };
