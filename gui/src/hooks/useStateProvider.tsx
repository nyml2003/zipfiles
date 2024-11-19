/* eslint-disable @typescript-eslint/no-explicit-any */
import React, { useState, createContext, ReactNode, FC } from "react";

type Reducer<State> = (state: State, ...args: any[]) => void;

type Action<Args extends any[]> = (...args: Args) => void;

type WithoutFirstArgument<F> = F extends (...args: infer A) => any
  ? A extends [infer _, ...infer Rest]
    ? Rest
    : []
  : never;

const useStateProvider = <
  State extends object,
  Reducers extends Record<string, Reducer<State>>,
>(
  reducers: Reducers,
  defaultState: State,
) => {
  type ActionObject = {
    [K in keyof Reducers]: Action<WithoutFirstArgument<Reducers[K]>>;
  };
  type ContextValue = { state: State; actions: ActionObject };

  const Context = createContext<ContextValue>({ state: {} as State, actions: {} as ActionObject });

  const Provider: FC<{ children: ReactNode; initialState?: Partial<State> }> = ({
    children,
    initialState,
  }) => {
    const [state, setState] = useState({ ...defaultState, ...initialState });

    const createActions = (actionMap: Reducers): ActionObject =>
      Object.fromEntries(
        Object.entries(actionMap).map(([key, action]) => [
          key,
          (...args: WithoutFirstArgument<Reducer<State>>) => {
            setState(prevState => {
              const state = { ...prevState };
              action(state, ...args);
              return state;
            });
          },
        ]),
      ) as ActionObject;

    const actions = createActions(reducers);

    return <Context.Provider value={{ state, actions }}>{children}</Context.Provider>;
  };

  return { Context, Provider };
};

export default useStateProvider;
