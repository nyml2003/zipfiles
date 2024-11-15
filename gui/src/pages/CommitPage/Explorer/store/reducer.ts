import { buildTree } from "@/utils";
import { State } from "./types";
import { FileDetail } from "./types";

export const updatePath = (state: State, payload: string) => {
  return { ...state, path: payload };
};

export const updateFile = (state: State, payload: string) => {
  return { ...state, file: payload };
};

export const updateFiles = (state: State, payload: FileDetail[]) => {
  const newFiles = buildTree(payload);
  return { ...state, files: newFiles, path: "" };
};
