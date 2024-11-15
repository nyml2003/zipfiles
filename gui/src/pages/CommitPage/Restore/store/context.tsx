import React, { useState, createContext, ReactNode } from "react";
import * as reducers from "./reducer";
import { State } from "./types";

const defaultState: State = {
  path: "",
  commitId: "",
  isEncrypt: false,
  chosenCommitId: "",
  fresh: false,
};

type ContextValue = { state: State; actions: typeof reducers };

export const Context = createContext<ContextValue>({
  state: defaultState,
  actions: {} as typeof reducers,
});

const Provider: React.FC<{ children: ReactNode; initialState?: Partial<State> }> = ({
  children,
  initialState,
}) => {
  const [state, setState] = useState({ ...defaultState, ...initialState });
  // 取出reducers对象的key和value
  const actions = Object.fromEntries(
    Object.entries(reducers).map(([key, reducerFn]) => {
      const actionName = key as keyof typeof reducers;
    }),
  ) as typeof
  return <Context.Provider value={{ state, actions }}>{children}</Context.Provider>;
};

export function createProvider(children: ReactNode, initialState?: Partial<State>) {
  return <Provider initialState={initialState}>{children}</Provider>;
}
