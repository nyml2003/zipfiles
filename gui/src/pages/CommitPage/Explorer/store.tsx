import { buildTree } from "@/utils";
import { FileType } from "@/types";
import { Dir } from "@/utils";
import useStateProvider from "@/hooks/useStateProvider";
import React, { ReactNode } from "react";

interface FileDetail {
  name: string;
  type: FileType;
  createTime: number;
  updateTime: number;
  size: number;
  owner: string;
  group: string;
  mode: number;
  path: string;
}

interface State {
  path: string;
  file: string;
  files: Dir<FileDetail>;
}
const defaultState: State = {
  path: "",
  file: "",
  files: {
    name: "",
    children: [],
    subDir: []
  }
};

const reducers = {
  updatePath: (state: State, payload: string) => {
    state.path = payload;
  },

  updateFile: (state: State, payload: string) => {
    state.file = payload;
  },

  updateFiles: (state: State, payload: FileDetail[]) => {
    state.files = buildTree(payload);
  }
};

const { Context, Provider } = useStateProvider(reducers, defaultState);
export const createProvider = (children: ReactNode, initialState?: Partial<State>) => {
  return <Provider initialState={initialState}>{children}</Provider>;
};
export { Context };
