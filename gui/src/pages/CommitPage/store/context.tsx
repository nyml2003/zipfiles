import React, { useState, createContext, ReactNode } from 'react';
import * as reducers from './reducer';
import { PayloadAction, State } from './types';

type Actions = {
  [K in keyof typeof reducers]: (action: PayloadAction) => void;
};

const defaultState: State = {
  path: '',
  file: '',
  commitId: '',
  files: {
    path: '',
    name: '',
    children: [],
  },
};

type ContextValue = { state: State; actions: Actions };

export const Context = createContext<ContextValue>({
  state: defaultState,
  actions: {} as Actions,
});

export const ExplorerProvider: React.FC<{ children: ReactNode; initialState?: Partial<State> }> = ({
  children,
  initialState,
}) => {
  const [state, setState] = useState({ ...defaultState, ...initialState });
  const actions = Object.fromEntries(
    Object.entries(reducers).map(([key, reducerFn]) => {
      const actionName = key as keyof Actions;
      return [
        actionName,
        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        (action: PayloadAction<any>) => {
          setState(state => reducerFn(state, action));
        },
      ];
    }),
  ) as unknown as Actions;

  return <Context.Provider value={{ state, actions }}>{children}</Context.Provider>;
};

export function createProvider(children: ReactNode, initialState?: Partial<State>) {
  return <ExplorerProvider initialState={initialState}>{children}</ExplorerProvider>;
}

export default ExplorerProvider;
