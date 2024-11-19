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
  commitId: string;
  files: Dir<FileDetail>;
  loading: boolean;
}
const defaultState: State = {
  path: "",
  file: "",
  commitId: "",
  files: {
    name: "",
    children: [],
    subDir: [],
  },
  loading: false,
};

const reducers = {
  updatePath: (state: State, payload: string) => {
    state.path = payload;
  },

  updateFile: (state: State, payload: string) => {
    console.log("updateFile", state, payload);
    state.file = payload;
  },

  updateFiles: (state: State, payload: FileDetail[]) => {
    state.files = buildTree(payload);
  },
};

const { Context, Provider } = useStateProvider(reducers, defaultState);
export const createProvider = (children: ReactNode, initialState?: Partial<State>) => {
  return <Provider initialState={initialState}>{children}</Provider>;
};
export { Context };
