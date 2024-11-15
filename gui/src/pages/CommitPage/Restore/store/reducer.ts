import { State } from "./types";

export const updatePath = (state: State, payload: string) => {
  return { ...state, path: payload };
};

export const setChosenPath = (state: State, payload: string) => {
  return { ...state, chosenPath: payload };
};

export const refresh = (state: State) => {
  return { ...state, fresh: !state.fresh };
};

