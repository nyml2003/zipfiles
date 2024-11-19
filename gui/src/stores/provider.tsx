import { Provider } from "react-redux";
import store from "./store";
import React from "react";

import { ReactNode } from "react";

export const ReduxProvider = ({ children }: { children: ReactNode }) => {
  return <Provider store={store}>{children}</Provider>;
};
