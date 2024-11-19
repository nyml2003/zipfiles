import useStateProvider from "@/hooks/useStateProvider";
import React, { ReactNode } from "react";

interface State {
  path: string; // 当前路径
  commitId: string; // commitId
  isEncrypt: boolean; // 是否加密
  chosenCommitId: string; // 选中的commitId
  fresh: boolean; // 是否刷新
  file: string; // 文件名
}

const defaultState: State = {
  path: "",
  commitId: "",
  isEncrypt: false,
  chosenCommitId: "",
  fresh: false,
  file: "",
};

const updatePath = (state: State, payload: string) => {
  state.path = payload;
  state.file = defaultState.file;
};

const setChosenPath = (state: State, payload: string) => {
  state.chosenCommitId = payload;
};

const refresh = (state: State) => {
  state.fresh = !state.fresh;
};

const initial = (state: State) => {
  state.path = defaultState.path;
  state.file = defaultState.file;
};

const updateFile = (state: State, payload: string) => {
  state.file = payload;
};

const reducers = {
  updatePath,
  setChosenPath,
  refresh,
  initial,
  updateFile,
};

const { Context, Provider } = useStateProvider(reducers, defaultState);
export const createProvider = (children: ReactNode, initialState?: Partial<State>) => {
  return <Provider initialState={initialState}>{children}</Provider>;
};
export { Context };
